// ------------------------------  Glint ---------------------------------
//    File: "IndSetDefs.h"
//
//    Definitions of standard indicators
//    Author: Alexander Vechersky, 2010 
// -------------------------------------------------------------------------

#ifndef IndSetDefs_h
#define  IndSetDefs_h

///////////////////////////////////
// Flags: INDSW_ADJUST_MAX | INDSW_MAX_ABSOLUTE | INDSW_FULL_SET_ONLY
//           INDSW_STANDARD == 0 
// Colors: CLR_CYAN
#define ADIST  m_indicSets.AddTail( new CIndicatorSettings

// **************************************************************
//      A L L     S Y S T E M S
// **************************************************************

#define SS_CPU      ADIST("Processor", "% Processor Time", "", CLR_WHITE,  INDSW_MAX_ABSOLUTE,  "CPU", 100.0 ) );   
#define SS_CPU_PRVG ADIST("Processor", "% Privileged Time", "", RGB(0xff,0x6c,0x83),  INDSW_MAX_ABSOLUTE,   "CPUp", 100.0 ) );  
#define SS_INTS     ADIST("Processor", "Interrupts/sec", "",  CLR_YELLOW, INDSW_FULL_SET_ONLY | INDSW_ADJUST_MAX , "Ints", 5*1000.0) );
#define SS_CPU_PERF ADIST("ProcessorPerformance", "frequency", "PPM_Processor_0",  RGB(230, 75, 75), INDSW_FULL_SET_ONLY | INDSW_LOCAL_VARIATIONS, "Freq", 3.2*1000.0) );
#define SS_PAGES    ADIST("Memory", "Pages/Sec", "",  RGB(203, 250, 16), INDSW_ADJUST_MAX, "Pags", 100.0) );  
#define SS_PGFLT    ADIST("Memory", "Page Faults/sec", "", RGB(240, 160, 160), INDSW_FULL_SET_ONLY | INDSW_ADJUST_MAX | INDSW_NOT_IN_MINIBAR, "PgFlt", 8*1000.0) );        

#define SS_CONTXT_SWTCH ADIST("System", "Context Switches/sec", "", CLR_YELLOW, INDSW_ADJUST_MAX, "Swch", 2*1000.0) );
#define SS_SYSCALLS     ADIST("System", "System Calls/sec", "", CLR_YELLOW_DARK, INDSW_FULL_SET_ONLY | INDSW_ADJUST_MAX, "SysC", 50*1000.0) );
#define SS_EXCEPTNS     ADIST("System", "Exception Dispatches/sec", "", RGB(240, 85, 85), INDSW_FULL_SET_ONLY | INDSW_ADJUST_MAX | INDSW_NOT_IN_MINIBAR, "Excp", 5.0) );
#define SS_CPUQUELEN    ADIST("System", "Processor Queue Length", "", CLR_YELLOW, INDSW_FULL_SET_ONLY | INDSW_LOCAL_VARIATIONS, "CPqu", 8*1000.0) );
#define SS_ALIGN_FIX    ADIST("System", "Alignment Fixups/sec", "", RGB(0xef,0xf3,0x3a), INDSW_FULL_SET_ONLY | INDSW_ADJUST_MAX, "AlgFx", 1.0) );

#define SS_CACHE_MAPS  ADIST("Cache", "Data Maps/sec", "", RGB(154, 239, 118),INDSW_ADJUST_MAX, "ChMps", 1000.0) );
#define SS_CACHE_FLSH  ADIST("Cache", "Data Flush Pages/sec", "", RGB(154, 239, 118), INDSW_FULL_SET_ONLY | INDSW_ADJUST_MAX, "ChFsh", 100.0) );
#define SS_CACHE_BYTES  ADIST("Memory", "Cache bytes", "", RGB(232, 202, 98), INDSW_LOCAL_VARIATIONS, "Cache", 100*1000*1000.0 ) );        
#define SS_CACHE_FAULTS ADIST("Memory", "Cache Faults/sec", "", RGB(251, 135, 60), INDSW_FULL_SET_ONLY | INDSW_LOCAL_VARIATIONS | INDSW_NOT_IN_MINIBAR, "ChFlt", 0.0) );        
#define SS_CACHE_FREAD  ADIST("Cache", "Fast Reads/sec", "",  CLR_YELLOW,  INDSW_FULL_SET_ONLY, "ChFst", 1000.0) );

#define SS_FILE_WRITES  ADIST("System", "File Write Bytes/sec", "", RGB(0x4f,0xff,0x7b), INDSW_ADJUST_MAX, "Write", 1*1000*1000.0) );
#define SS_FILE_READS   ADIST("System", "File Read Bytes/sec", "", RGB(0,0xc4,0x62), INDSW_ADJUST_MAX, "Read", 1*1000*1000.0) );
#define SS_FILE_OPS     ADIST("System", "File Data Operations/sec", "", RGB(0,0xe4,0x82), INDSW_ADJUST_MAX | INDSW_LOCAL_VARIATIONS, "Fops", 0.0) );
#define SS_FILE_CTRL_OP ADIST("System", "File Control Operations/sec", "", CLR_YELLOW, 0, "FCIO", 100.0) );

#define SS_PHDISK       ADIST("PhysicalDisk", "Disk Bytes/sec", "",CLR_GREEN,INDSW_FULL_SET_ONLY, "Disk", 5*1000*1000.0) );
#define SS_PHDISK_QUE   ADIST("PhysicalDisk", "Current Disk Queue Length", "",RGB(0x80,0xc8,0x37), INDSW_FULL_SET_ONLY | INDSW_LOCAL_VARIATIONS, "DskQ", 0.0 ) );
#define SS_PHDISKR      ADIST("PhysicalDisk", "Disk Read Bytes/sec", "",RGB(3, 232, 190), INDSW_ADJUST_MAX, "DskR",  5*1000*1000.0) );
#define SS_PHDISKW      ADIST("PhysicalDisk", "Disk Write Bytes/sec", "",RGB(3, 252, 140), INDSW_ADJUST_MAX, "DskW",  5*1000*1000.0) );

#define SS_MEMAVL       ADIST("Memory", "Available bytes", "",  RGB(254, 174, 148),  INDSW_LOCAL_VARIATIONS, "Mem", 0.0) );        
#define SS_MEMC_OUT     ADIST("Memory", "Pages Output/sec", "", RGB(0xff,0x2b,0x3a), INDSW_FULL_SET_ONLY | INDSW_ADJUST_MAX, "PgOut", 0.0) );        

#define SS_OBJ_EVENTS   ADIST("Objects", "Events", "",  RGB(208,254,69), INDSW_FULL_SET_ONLY | INDSW_LOCAL_VARIATIONS | INDSW_NOT_IN_MINIBAR, "Evnts", 100) );      
#define SS_OBJ_THREADS  ADIST("Objects", "Threads", "", RGB(207, 188, 148), INDSW_FULL_SET_ONLY | INDSW_LOCAL_VARIATIONS, "Thrds", 0.0) );        
#define SS_OBJ_MUTEX    ADIST("Objects", "Mutexes", "", RGB(248,164,60), INDSW_FULL_SET_ONLY | INDSW_LOCAL_VARIATIONS | INDSW_NOT_IN_MINIBAR, "Mutxs", 100) );      

        // .......... NETWORK .............

#define SS_NETWK_INTF   ADIST("Network Interface", "Bytes Total/sec", "", RGB(0xc20,0x45,0xf8),INDSW_FULL_SET_ONLY | INDSW_ADJUST_MAX, "Neti", 5*1000.0) );  // NT4, Win2000
#define SS_NETWK_RECVD  ADIST("Network Interface", "Bytes Received/sec", "", RGB(125,125,250), INDSW_FULL_SET_ONLY | INDSW_ADJUST_MAX , "netR", 3*1000.0) );  // XP
#define SS_NETWK_SEND   ADIST("Network Interface", "Bytes Sent/sec", "", RGB(208,90,250), INDSW_FULL_SET_ONLY |  INDSW_ADJUST_MAX, "netS",3*1000.0) );  // XP
#define SS_NETWK_RCV_ERR  ADIST("Network Interface", "Packets Received Errors", "", RGB(125,125,250), INDSW_ADJUST_MAX, "rcErr", 20*1000.0) );  // XP
#define SS_NETWK_SND_ERR  ADIST("Network Interface", "Packets Outbound Errors", "", RGB(208,90,250), INDSW_ADJUST_MAX, "snErr",20*1000.0) );  // XP

//#define SS_NWLNK_TOTAL  ADIST("NWLink NetBIOS", "Bytes Total/sec", "", RGB(196,32,196),INDSW_FULL_SET_ONLY | INDSW_ADJUST_MAX, "NWlk", 5*1000.0) );    
//#define SS_NWLNK_IPX    ADIST("NWLink IPX", "Datagram Bytes/sec", "", RGB(89,190,214), INDSW_FULL_SET_ONLY | INDSW_ADJUST_MAX, "IPX", 5*1000.0) );
//#define SS_NWLNK_SPX    ADIST("NWLink SPX", "Datagram Bytes/sec", "", RGB(139,133,214), INDSW_FULL_SET_ONLY | INDSW_ADJUST_MAX, "SPX", 5*1000.0) );

#define SS_IP_TOTL  ADIST("IP", "Datagrams/sec", "", RGB(168,51,247),INDSW_FULL_SET_ONLY | INDSW_ADJUST_MAX, "IPdt", 10*10.0) );                 // costly
#define SS_IP_RCVD  ADIST("IP", "Datagrams Sent/sec", "", RGB(0x71,0xdc,0xff),INDSW_FULL_SET_ONLY | INDSW_ADJUST_MAX, "IPrc", 5*10.0) );      // costly
#define SS_IP_SEND  ADIST("IP", "Datagrams Received/sec", "", RGB(0x71,0xb8,0xff), INDSW_FULL_SET_ONLY | INDSW_ADJUST_MAX, "IPsd", 5*10.0) ); // costly

#define SS_TCP_SEGS         ADIST("TCP", "Segments/sec", "", RGB(136,176,244),INDSW_FULL_SET_ONLY | INDSW_ADJUST_MAX, "TCPs", 2*100.0) );  // costly
#define SS_TCPSEGS_RECVD    ADIST("TCP", "Segments Received/sec", "",   CLR_BLUE_LIGHT,  INDSW_FULL_SET_ONLY | INDSW_ADJUST_MAX , "TcpI", 2*100.0) );     // costly
#define SS_TCPSEGS_SENT     ADIST("TCP", "Segments Sent/sec", "", CLR_BLUE_LIGHT, INDSW_FULL_SET_ONLY | INDSW_ADJUST_MAX , "TcpO", 2*100.0) );        // costly

        // ......... Server ..........

#define SS_XP_COMTRANS  ADIST("Distributed Transaction Coordinator", "Committed Transactions/sec", "", RGB(154, 239, 118), INDSW_FULL_SET_ONLY | INDSW_ADJUST_MAX, "DTC", 10.0) );
#define SS_REDIRECTOR_TOT   ADIST("Redirector", "Bytes Total/sec", "",  RGB(217,51,200),  INDSW_FULL_SET_ONLY | INDSW_ADJUST_MAX , "Redir", 3*1000.0) );       // costly

// **************************************************************
//      W I N    N T 4
// **************************************************************

#define SS_INTS_NT4 ADIST("Processor", "Total Interrupts/sec", "",  CLR_YELLOW, INDSW_FULL_SET_ONLY | INDSW_ADJUST_MAX, "Ints", 100000.0) ); // duplicate for some systems
#define SS_CPU_NT4      ADIST("Processor", "% Total Processor Time", "", CLR_WHITE,  INDSW_MAX_ABSOLUTE, "CPU", 100.0 ) );   // duplicate for some systems
#define SS_NETWK_SEGM   ADIST("Network Segment", "% Network utilization", "",CLR_CYAN,  INDSW_FULL_SET_ONLY, "Net", 100.0) );   // costly

///////////////////////////////////
#endif