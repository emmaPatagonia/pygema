/****************************************************************************************
*       RCS Standard Header
*
*$Source: /geophys/develop/applications/pitsa/include/RCS/lsd.h,v $
*
*$Author: andreas $
*
*$State: Exp $
*
*$Log: lsd.h,v $
*Revision 1.4  1996/08/08 00:51:11  andreas
*entered defined in #elif SUNOS statement
*
*Revision 1.3  1996/08/08 00:44:39  andreas
*removed one blank in #elif SUNOS ... there was a parse error!
*
*Revision 1.2  1996/08/05 13:56:38  andreas
*Entered Solaris version.
*
*Revision 1.1  1996/02/06 13:32:10  andreas
*Initial revision
*
*
*$Locker:  $
*
*
*****************************************************************************************/
/*	Linux
	Sun
	DOS

	Systemabhaengige  DEFINES

	G.Asch	7.7.93			*/




#ifdef SOLARIS
#undef INTEL
#define MKTIME mktime
#elif defined SUNOS
#undef INTEL
#define MKTIME timelocal
#elif defined  LINUX
#define INTEL  1
#define MKTIME mktime
#elif defined  MSDOS
#define INTEL  1
#define MKTIME mktime
#endif
