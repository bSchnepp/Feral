/*
Copyright (c) 2018, Brian Schnepp

Permission is hereby granted, free of charge, to any person or organization 
obtaining  a copy of the software and accompanying documentation covered by 
this license (the "Software") to use, reproduce, display, distribute, execute, 
and transmit the Software, and to prepare derivative works of the Software, 
and to permit third-parties to whom the Software is furnished to do so, all 
subject to the following:

The copyright notices in the Software and this entire statement, including 
the above license grant, this restriction and the following disclaimer, must 
be included in all copies of the Software, in whole or in part, and all 
derivative works of the Software, unless such copies or derivative works are 
solely in the form of machine-executable object code generated by a source 
language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE FOR ANY 
DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE, ARISING 
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
IN THE SOFTWARE.
 */
 
#ifndef _FERAL_RENEGADE_PROTOCOL_H_
#define _FERAL_RENEGADE_PROTOCL_H_

/*	
	Address space is 512 bits, or the same as IPv6 squared twice. (10 billion people could have 10 billion devices and still not 
	even come anywhere remotely close to running out of unique addresses.)
*/

/* IPv6 will eventually run out of addresses, and if we learned anything from IPv4, it's that 2^32^2^2 isn't going to be enough.	*/

/*
	The intent is that this number is so large that for every single planet in the observable universe can have a number of users 
	on said planet equal to the number of planets in the observable universe, and still have more than plenty left over.
	In other words, it would take a lot in order to completely exhaust all the addresses possible in RENEGADE.
*/

/*
	Essentially, the intent is to build a brand new internet with none of the space limitations (far greater address space than IPv6),
	version control, persistence (ala BitTorrent), peer-to-peer architecture, reliable built-in domain name system (no DNS hijacking), etc. etc.
	
	It eliminates the notion of an "internet service provider" by forcing all nodes to peer with a certain number of others (at least 4):
	thus, the collection of a small handful of big service providers is entirely impossible: you could just start your own
	service provider and there would be nothing they could do about it (other than try to throttle connection to you, but that'd be
	a beyond-horrible business practice: you could go around whoever is throttling you and so on. All they'd do is cut their own profits.).
	
	More or less similar to the interplanetary file system, but *specifically* focused on being a network protocol you could
	in theory run TCP or HTTP  or whatever over. RENEGADE is not a file system, nor delivery network, but *specifically* a network protocol.
	
	Maybe consider aborbing IPv4 into this: the underlying stuff is actually ordinary IPv4 protocols sent around to specific
	parts of the network, meaning it's backwards compatible??? (ie, RENEGADE is actually a gigantic superset of IPv4???)
	
	RENEGADE replaces IP in the OSI model (layer 3).
	RENEGADE does not check for double arrival, and makes no promises about being delivered in order. (That's TCP's job.)
	Essentially right now it's a mess of ideas without any formal structure yet.

	All packets sent in RENEGADE must be sent to all nodes peered with (by basic math, all packets are duplicated at least 4 times).
	RENEGADE makes no guarantee of actual delivery, being build on a "best effort" model.
 */

#include <feral/stdtypes.h>

/* For now, we trust CryptoNight v7 since it's memory-bound and hard to create useful ASICs for.*/
typedef struct renegade_version_one_blockchain
{
	CHAR	CryptoNightAddress[95];
	/* TODO */
}renegade_version_one_blockchain;

typedef UINT64	BlockchainID;
typedef UINT64	BlockchainVersion;

typedef struct renegade_protocol_header
{
	UINT64				ProtocolVersion;		/* What version of the protocol are we using? */
	BlockchainID			BChainAlgo;			/* What blockchain protocol/hashing algo is in use (CryptoNight, SHA256, etc.) */
	BlockchainVersion		BChainVersion;			/* What revision of this blockchain do we trust? */
	UINT8				Address[64];			/* The address we're targetting, being 64 8-bit values. */
	UINT32				Crc32;				/* Validate the address being used above. */

	UINT64				NanoEpochTimePacketValidHigher;		/* What time should this packet be allowed to circulate? (Typically initial broadcast time) (nanoseconds) */
	UINT64				NanoEpochTimePacketValidLower;		/* What time should this packet be allowed to circulate? (Typically initial broadcast time) (nanoseconds) */

	UINT64				NanoEpochTimePacketInvalidHigher;	/* Time when this packet expires (nanoseconds) */
	UINT64				NanoEpochTimePacketValidLower;		/* What time should this packet be allowed to circulate? (Typically initial broadcast time) (nanoseconds) */
	UINT32				Crc32Packet;				/* Validate the integrity of this packet. */
}renegade_protocol_header;

#endif
