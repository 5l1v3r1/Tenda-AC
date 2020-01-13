/*
 * Copyright (c) 1982, 1986, 1988, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD$
 */

#ifndef INET
//#error "IPDIVERT requires INET."
#endif

#include <sys/param.h>
#if !defined(CONFIG_RTL_819X)	//jwj:20120612
#include <sys/kernel.h>
#endif
#include <sys/malloc.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/protosw.h>
#include <sys/socketvar.h>
#include <sys/sysctl.h>
#if !defined(CONFIG_RTL_819X)	//jwj:20120612
#include <sys/systm.h>
#include <vm/vm_zone.h>
#include <sys/proc.h>
#endif

#include <net/if.h>
#include <net/route.h>

#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/in_pcb.h>
#include <netinet/in_var.h>
#include <netinet/ip_var.h>
#if defined(CONFIG_RTL_VLAN_SUPPORT)&&!defined(CONFIG_RTL_NAPT_KERNEL_MODE_SUPPORT)
#include <netinet/rtl_vlan.h>
extern int rtl_vlan_support_enable;
#include <cyg/io/eth/rltk/819x/wrapper/timer.h>
#endif

/*
 * Divert sockets
 */

/*
 * Allocate enough space to hold a full IP packet
 */
#define	DIVSNDQ		(65536 + 100)
#define	DIVRCVQ		(65536 + 100)

#if defined(CONFIG_RTL_819X)	//jwj:20120612
#define	KASSERT(exp,msg)	do { if (!(exp)) panic msg; } while (0)
#endif
/*
 * A 16 bit cookie is passed to and from the user process.
 * The user process can send it back to help the caller know
 * something about where the packet originally came from.
 *
 * In the case of ipfw, then the cookie is the rule that sent
 * us here. On reinjection is is the rule after which processing
 * should continue. Leaving it the same will make processing start
 * at the rule number after that which sent it here. Setting it to
 * 0 will restart processing at the beginning. 
 *
 * For divert_packet(), ip_divert_cookie is an input value only.
 * For div_output(), ip_divert_cookie is an output value only.
 */
u_int16_t ip_divert_cookie;

/* Internal variables */
static struct inpcbhead divcb;
static struct inpcbinfo divcbinfo;

static u_long	div_sendspace = DIVSNDQ;	/* XXX sysctl ? */
static u_long	div_recvspace = DIVRCVQ;	/* XXX sysctl ? */

/* Optimization: have this preinitialized */
static struct sockaddr_in divsrc = { sizeof(divsrc), AF_INET };

#if defined(__ECOS) && defined(CONFIG_RTL_819X)//jwj:20120612
struct proc{
    int __unused;
};
#endif

/* Internal functions */
static int div_output(struct socket *so,
		struct mbuf *m, struct sockaddr *addr, struct mbuf *control);

/*
 * Initialize divert connection block queue.
 */
void
div_init(void)
{
	LIST_INIT(&divcb);
	divcbinfo.listhead = &divcb;
	/*
	 * XXX We don't use the hash list for divert IP, but it's easier
	 * to allocate a one entry hash list than it is to check all
	 * over the place for hashbase == NULL.
	 */
	divcbinfo.hashbase = hashinit(1, M_PCB, &divcbinfo.hashmask);
	divcbinfo.porthashbase = hashinit(1, M_PCB, &divcbinfo.porthashmask);
	#if defined(CONFIG_RTL_819X)
	divcbinfo.ipi_zone = zinit("divcb", sizeof(struct inpcb),
				   2, ZONE_INTERRUPT, 0);
	#else
	divcbinfo.ipi_zone = zinit("divcb", sizeof(struct inpcb),
				   maxsockets, ZONE_INTERRUPT, 0);
	#endif
}

/*
 * IPPROTO_DIVERT is not a real IP protocol; don't allow any packets
 * with that protocol number to enter the system from the outside.
 */
void
div_input(struct mbuf *m, int off, int proto)
{
	ipstat.ips_noproto++;
	m_freem(m);
}

/*
 * Divert a packet by passing it up to the divert socket at port 'port'.
 *
 * Setup generic address and protocol structures for div_input routine,
 * then pass them along with mbuf chain.
 */
void
divert_packet(struct mbuf *m, int incoming, int port)
{
	struct ip *ip;
	struct inpcb *inp;
	struct socket *sa;
	u_int16_t nport;
	#if defined(CONFIG_RTL_VLAN_SUPPORT)&&!defined(CONFIG_RTL_NAPT_KERNEL_MODE_SUPPORT)
	struct ip_info ipInfo;
	#endif

	/* Sanity check */
	KASSERT(port != 0, ("%s: port=0", __FUNCTION__));

	/* Record and reset divert cookie */
	divsrc.sin_port = ip_divert_cookie;
	ip_divert_cookie = 0;

	/* Assure header */
	if (m->m_len < sizeof(struct ip) &&
	    (m = m_pullup(m, sizeof(struct ip))) == 0) {
		return;
	}
	
	ip = mtod(m, struct ip *);

	#if defined(CONFIG_RTL_VLAN_SUPPORT)&&!defined(CONFIG_RTL_NAPT_KERNEL_MODE_SUPPORT)
	ipInfo.dir = incoming?2:1;	/*1: outbound, 2: inbound*/
	ipInfo.protocol = ip->ip_p;
	ipInfo.id = ip->ip_id;
	ipInfo.sip = ip->ip_src.s_addr;
	ipInfo.dip = ip->ip_dst.s_addr;
	ipInfo.now = (unsigned long)jiffies;
	if(rtl_vlan_support_enable)
		rtl_addTagForNatdEntry(&ipInfo, m->m_pkthdr.tag);
	#endif

	/*
	 * Record receive interface address, if any.
	 * But only for incoming packets.
	 */
	divsrc.sin_addr.s_addr = 0;
	if (incoming) {
		struct ifaddr *ifa;

		/* Sanity check */
		KASSERT((m->m_flags & M_PKTHDR), ("%s: !PKTHDR", __FUNCTION__));

		/* Find IP address for receive interface */
		TAILQ_FOREACH(ifa, &m->m_pkthdr.rcvif->if_addrhead, ifa_link) {
			if (ifa->ifa_addr == NULL)
				continue;
			if (ifa->ifa_addr->sa_family != AF_INET)
				continue;
			divsrc.sin_addr =
			    ((struct sockaddr_in *) ifa->ifa_addr)->sin_addr;
			break;
		}
	}
	/*
	 * Record the incoming interface name whenever we have one.
	 */
	bzero(&divsrc.sin_zero, sizeof(divsrc.sin_zero));
	if (m->m_pkthdr.rcvif) {
		/*
		 * Hide the actual interface name in there in the 
		 * sin_zero array. XXX This needs to be moved to a
		 * different sockaddr type for divert, e.g.
		 * sockaddr_div with multiple fields like 
		 * sockaddr_dl. Presently we have only 7 bytes
		 * but that will do for now as most interfaces
		 * are 4 or less + 2 or less bytes for unit.
		 * There is probably a faster way of doing this,
		 * possibly taking it from the sockaddr_dl on the iface.
		 * This solves the problem of a P2P link and a LAN interface
		 * having the same address, which can result in the wrong
		 * interface being assigned to the packet when fed back
		 * into the divert socket. Theoretically if the daemon saves
		 * and re-uses the sockaddr_in as suggested in the man pages,
		 * this iface name will come along for the ride.
		 * (see div_output for the other half of this.)
		 */ 
		snprintf(divsrc.sin_zero, sizeof(divsrc.sin_zero),
			"%s%d", m->m_pkthdr.rcvif->if_name,
			m->m_pkthdr.rcvif->if_unit);
	}

	/* Put packet on socket queue, if any */
	sa = NULL;
	nport = htons((u_int16_t)port);
	LIST_FOREACH(inp, &divcb, inp_list) {
		if (inp->inp_lport == nport)
			sa = inp->inp_socket;
	}
	if (sa) {
		if (sbappendaddr(&sa->so_rcv, (struct sockaddr *)&divsrc,
				m, (struct mbuf *)0) == 0)
			m_freem(m);
		else
			sorwakeup(sa);
	} else {
		m_freem(m);
		ipstat.ips_noproto++;
		ipstat.ips_delivered--;
        }
}

/*
 * Deliver packet back into the IP processing machinery.
 *
 * If no address specified, or address is 0.0.0.0, send to ip_output();
 * otherwise, send to ip_input() and mark as having been received on
 * the interface with that address.
 */
static int
div_output(so, m, addr, control)
	struct socket *so;
	register struct mbuf *m;
	struct sockaddr *addr;
	struct mbuf *control;
{
	register struct inpcb *const inp = sotoinpcb(so);
	register struct ip *const ip = mtod(m, struct ip *);
	struct sockaddr_in *sin = (struct sockaddr_in *)addr;
	int error = 0;

	#if defined(CONFIG_RTL_VLAN_SUPPORT)&&!defined(CONFIG_RTL_NAPT_KERNEL_MODE_SUPPORT)
	struct ip_info ipInfo;
	#endif

	if (control)
		m_freem(control);		/* XXX */

	/* Loopback avoidance and state recovery */
	if (sin) {
		int	len = 0;
		char	*c = sin->sin_zero;

		ip_divert_cookie = sin->sin_port;

		/*
		 * Find receive interface with the given name or IP address.
		 * The name is user supplied data so don't trust it's size or 
		 * that it is zero terminated. The name has priority.
		 * We are presently assuming that the sockaddr_in 
		 * has not been replaced by a sockaddr_div, so we limit it
		 * to 16 bytes in total. the name is stuffed (if it exists)
		 * in the sin_zero[] field.
		 */
		while (*c++ && (len++ < sizeof(sin->sin_zero)));
		if ((len > 0) && (len < sizeof(sin->sin_zero)))
			m->m_pkthdr.rcvif = ifunit(sin->sin_zero);
	} else {
		ip_divert_cookie = 0;
	}

	#if defined(CONFIG_RTL_VLAN_SUPPORT)&&!defined(CONFIG_RTL_NAPT_KERNEL_MODE_SUPPORT)
	if (!sin || sin->sin_addr.s_addr == 0)
		ipInfo.dir = 1;		/*1: outbound, 2: inbound*/
	else
		ipInfo.dir = 2;
	ipInfo.protocol = ip->ip_p;
	ipInfo.id = ip->ip_id;
	ipInfo.sip = ip->ip_src.s_addr;
	ipInfo.dip = ip->ip_dst.s_addr;
	ipInfo.now = (unsigned long)jiffies;
	if(rtl_vlan_support_enable)
		rtl_lookupTagForNatdEntry(&ipInfo, &(m->m_pkthdr.tag));
	#endif

	/* Reinject packet into the system as incoming or outgoing */
	if (!sin || sin->sin_addr.s_addr == 0) {
		/*
		 * Don't allow both user specified and setsockopt options,
		 * and don't allow packet length sizes that will crash
		 */
		if (((ip->ip_hl != (sizeof (*ip) >> 2)) && inp->inp_options) ||
		     ((u_short)ntohs(ip->ip_len) > m->m_pkthdr.len)) {
			error = EINVAL;
			goto cantsend;
		}

		/* Convert fields to host order for ip_output() */
		NTOHS(ip->ip_len);
		NTOHS(ip->ip_off);

		/* Send packet to output processing */
		ipstat.ips_rawout++;			/* XXX */
//		diag_printf("%s:%d\n",__FUNCTION__,__LINE__);
		error = ip_output(m, inp->inp_options, &inp->inp_route,
			(so->so_options & SO_DONTROUTE) |
			IP_ALLOWBROADCAST | IP_RAWOUTPUT,
			inp->inp_moptions);
	} else {
		struct	ifaddr *ifa;

		/* If no luck with the name above. check by IP address.  */
		if (m->m_pkthdr.rcvif == NULL) {
			/*
			 * Make sure there are no distractions
			 * for ifa_ifwithaddr. Clear the port and the ifname.
			 * Maybe zap all 8 bytes at once using a 64bit write?
			 */
			bzero(sin->sin_zero, sizeof(sin->sin_zero));
			/* *((u_int64_t *)sin->sin_zero) = 0; */ /* XXX ?? */
			sin->sin_port = 0;
			if (!(ifa = ifa_ifwithaddr((struct sockaddr *) sin))) {
				error = EADDRNOTAVAIL;
				goto cantsend;
			}
			m->m_pkthdr.rcvif = ifa->ifa_ifp;
		}
//		diag_printf("%s:%d\n",__FUNCTION__,__LINE__);
		/* Send packet to input processing */
		ip_input(m);
	}

	/* paranoid: Reset for next time (and other packets) */
	/* almost definitly already done in the ipfw filter but.. */
	ip_divert_cookie = 0;
	return error;

cantsend:
	m_freem(m);
	ip_divert_cookie = 0;
	return error;
}

static int
div_attach(struct socket *so, int proto, struct proc *p)
{
	struct inpcb *inp;
	int error, s;

	inp  = sotoinpcb(so);
	if (inp)
		panic("div_attach");
	
	#if defined(CONFIG_RTL_819X)	//jwj:20120612
	#ifndef __ECOS
	if (p && (error = suser(p)) != 0)
		return error;
	#endif
	#endif

	error = soreserve(so, div_sendspace, div_recvspace);
	if (error)
		return error;
	s = splnet();
	error = in_pcballoc(so, &divcbinfo, p);
	splx(s);
	if (error)
		return error;
	inp = (struct inpcb *)so->so_pcb;
	inp->inp_ip_p = proto;
	inp->inp_vflag |= INP_IPV4;
	inp->inp_flags |= INP_HDRINCL;
	/* The socket is always "connected" because
	   we always know "where" to send the packet */
	so->so_state |= SS_ISCONNECTED;
	return 0;
}

static int
div_detach(struct socket *so)
{
	struct inpcb *inp;

	inp = sotoinpcb(so);
	if (inp == 0)
		panic("div_detach");
	in_pcbdetach(inp);
	return 0;
}

static int
div_abort(struct socket *so)
{
	soisdisconnected(so);
	return div_detach(so);
}

static int
div_disconnect(struct socket *so)
{
	if ((so->so_state & SS_ISCONNECTED) == 0)
		return ENOTCONN;
	return div_abort(so);
}

static int
div_bind(struct socket *so, struct sockaddr *nam, struct proc *p)
{
	struct inpcb *inp;
	int s;
	int error;

	s = splnet();
	inp = sotoinpcb(so);
	/* in_pcbbind assumes that the socket is a sockaddr_in
	* and in_pcbbind requires a valid address. Since divert
	* sockets don't we need to make sure the address is
	* filled in properly.
	* XXX -- divert should not be abusing in_pcbind
	* and should probably have its own family.
	*/
	if (nam->sa_family != AF_INET) {
		error = EAFNOSUPPORT;
	} else {
               ((struct sockaddr_in *)nam)->sin_addr.s_addr = INADDR_ANY;
		error = in_pcbbind(inp, nam, p);
	}
	splx(s);
	return error;
}

static int
div_shutdown(struct socket *so)
{
	socantsendmore(so);
	return 0;
}

static int
div_send(struct socket *so, int flags, struct mbuf *m, struct sockaddr *nam,
	 struct mbuf *control, struct proc *p)
{
	/* Packet must have a header (but that's about it) */
	if (m->m_len < sizeof (struct ip) &&
	    (m = m_pullup(m, sizeof (struct ip))) == 0) {
		ipstat.ips_toosmall++;
		m_freem(m);
		return EINVAL;
	}

	/* Send packet */
	return div_output(so, m, nam, control);
}

static int
div_pcblist(SYSCTL_HANDLER_ARGS)
{
	int error, i, n, s;
	struct inpcb *inp, **inp_list;
	inp_gen_t gencnt;
	struct xinpgen xig;

	/*
	 * The process of preparing the TCB list is too time-consuming and
	 * resource-intensive to repeat twice on every request.
	 */
	if (req->oldptr == 0) {
		n = divcbinfo.ipi_count;
		req->oldidx = 2 * (sizeof xig)
			+ (n + n/8) * sizeof(struct xinpcb);
		return 0;
	}

	if (req->newptr != 0)
		return EPERM;

	/*
	 * OK, now we're committed to doing something.
	 */
	s = splnet();
	gencnt = divcbinfo.ipi_gencnt;
	n = divcbinfo.ipi_count;
	splx(s);

	xig.xig_len = sizeof xig;
	xig.xig_count = n;
	xig.xig_gen = gencnt;
	xig.xig_sogen = so_gencnt;
	error = SYSCTL_OUT(req, &xig, sizeof xig);
	if (error)
		return error;

	inp_list = malloc(n * sizeof *inp_list, M_TEMP, M_WAITOK);
	if (inp_list == 0)
		return ENOMEM;
	
	s = splnet();
	for (inp = LIST_FIRST(divcbinfo.listhead), i = 0; inp && i < n;
	     inp = LIST_NEXT(inp, inp_list)) {
		if (inp->inp_gencnt <= gencnt
			#if !defined(CONFIG_RTL_819X)	//jwj:20120626
			&& !prison_xinpcb(req->p, inp)
			#endif
			)
			inp_list[i++] = inp;
	}
	splx(s);
	n = i;

	error = 0;
	for (i = 0; i < n; i++) {
		inp = inp_list[i];
		if (inp->inp_gencnt <= gencnt) {
			struct xinpcb xi;
			xi.xi_len = sizeof xi;
			/* XXX should avoid extra copy */
			bcopy(inp, &xi.xi_inp, sizeof *inp);
			if (inp->inp_socket)
				sotoxsocket(inp->inp_socket, &xi.xi_socket);
			error = SYSCTL_OUT(req, &xi, sizeof xi);
		}
	}
	if (!error) {
		/*
		 * Give the user an updated idea of our state.
		 * If the generation differs from what we told
		 * her before, she knows that something happened
		 * while we were processing this request, and it
		 * might be necessary to retry.
		 */
		s = splnet();
		xig.xig_gen = divcbinfo.ipi_gencnt;
		xig.xig_sogen = so_gencnt;
		xig.xig_count = divcbinfo.ipi_count;
		splx(s);
		error = SYSCTL_OUT(req, &xig, sizeof xig);
	}
	free(inp_list, M_TEMP);
	return error;
}

SYSCTL_DECL(_net_inet_divert);
SYSCTL_PROC(_net_inet_divert, OID_AUTO, pcblist, CTLFLAG_RD, 0, 0,
	    div_pcblist, "S,xinpcb", "List of active divert sockets");

struct pr_usrreqs div_usrreqs = {
	div_abort, pru_accept_notsupp, div_attach, div_bind,
	pru_connect_notsupp, pru_connect2_notsupp, in_control, div_detach,
	div_disconnect, pru_listen_notsupp, in_setpeeraddr, pru_rcvd_notsupp,
	pru_rcvoob_notsupp, div_send, pru_sense_null, div_shutdown,
	in_setsockaddr, sosend, soreceive, sopoll
};