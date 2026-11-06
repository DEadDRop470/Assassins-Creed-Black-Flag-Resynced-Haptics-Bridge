#pragma once
#include "compat_tables.h"

enum CompatTargetMode : u32 { COMPAT_NONE,COMPAT_106,COMPAT_107,COMPAT_108,COMPAT_AUTO };
static u32 g_compatTargetMode{};
static CompatStateLocation g_autoStateLocations[21]{};
static const CompatStateLocation* g_selectedStateLocations{};
static u32 g_autoStateMatches[13]{};
static u64 g_autoStateTables[13]{};
static u64 g_autoCandidateRows{};

static u64 selected_state_slot(const StateHookTarget& target,bool use107) {
    return g_selectedStateLocations ? g_selectedStateLocations[&target-kStateHookTargets].slot :
                                     (use107?target.slot107:target.slot106);
}
static u64 selected_state_function(const StateHookTarget& target,bool use107) {
    return g_selectedStateLocations ? g_selectedStateLocations[&target-kStateHookTargets].function :
                                     (use107?target.function107:target.function106);
}
static const char* compat_target_mode_name() {
    switch(g_compatTargetMode) {
    case COMPAT_106:return "verified 1.06-compatible profile";
    case COMPAT_107:return "verified 1.07 profile";
    case COMPAT_108:return "verified supplied 1.08 profile";
    case COMPAT_AUTO:return "automatic signature/table discovery";
    default:return "unresolved";
    }
}
static u64 compat_follow_code(u64 address) {
    for(u32 depth=0;depth<5;++depth) {
        if(address<(u64)g_gameBase ||
           !compat_memory_range(address-(u64)g_gameBase,5,true))return 0;
        const u8* p=(const u8*)address;
        if(p[0]==0xE9)address=(u64)((i64)address+5+(i64)*(const i32*)(p+1));
        else if(p[0]==0xEB)address=(u64)((i64)address+2+(i64)*(const signed char*)(p+1));
        else return address;
    }
    return 0;
}
static bool compat_pattern_matches(u64 address,u16 id) {
    if(id>=sizeof(kCompatCodePatterns)/sizeof(kCompatCodePatterns[0]))return false;
    address=compat_follow_code(address);if(!address)return false;
    const auto& pattern=kCompatCodePatterns[id];
    if(!pattern.length || !compat_memory_range(address-(u64)g_gameBase,pattern.length,true))return false;
    const u8* code=(const u8*)address;
    for(u32 i=0;i<pattern.length;++i)
        if(((code[i]^pattern.bytes[i])&pattern.mask[i])!=0)return false;
    return true;
}
template<u32 N>static bool compat_any_pattern(u64 address,const u16 (&ids)[N]) {
    for(u32 i=0;i<N;++i)if(compat_pattern_matches(address,ids[i]))return true;
    return false;
}
static bool compat_model_matches(const u64* methods,const CompatStateModel& model) {
    static const u8 order[9]={7,8,0,1,2,3,4,5,6};
    for(u32 bank=0;bank<2;++bank) {
        bool valid=true;
        for(u32 i=0;i<9;++i) {
            u32 method=order[i];
            if(!compat_pattern_matches(methods[method],model.bank[bank][method])){valid=false;break;}
        }
        if(valid)return true;
    }
    return false;
}
static void compat_scan_data_span(u64 rva,u64 length) {
    u64 at=(u64)g_gameBase+rva,end=at+length;
    while(at<end) {
        CompatMemoryInfo m{};
        if(g_api.VirtualQuery((void*)at,&m,sizeof(m))!=sizeof(m))return;
        u64 next=(u64)m.BaseAddress+m.RegionSize;
        if((u64)m.BaseAddress>at || next<=at)return;
        if(next>end)next=end;
        if(compat_region_allowed(m,false)) {
            u64 aligned=(at+7ull)&~7ull;
            for(u64 p=aligned;p<next && p<=end && end-p>=9*sizeof(u64);p+=8) {
                if(next-p<9*sizeof(u64) &&
                   !compat_address_range((void*)p,9*sizeof(u64),false))continue;
                const u64* methods=(const u64*)p;
                u64 fn=methods[6];
                if(fn<(u64)g_gameBase || fn-(u64)g_gameBase>=g_gameSize ||
                   !compat_any_pattern(fn,kCompatFilter6))continue;
                if(!compat_any_pattern(methods[3],kCompatFilter3) ||
                   !compat_any_pattern(methods[4],kCompatFilter4))continue;
                ++g_autoCandidateRows;
                for(u32 i=0;i<13;++i) {
                    if(!compat_model_matches(methods,kCompatStateModels[i]))continue;
                    u64 table=p-(u64)g_gameBase;
                    if(g_autoStateMatches[i] && g_autoStateTables[i]==table)continue;
                    if(g_autoStateMatches[i]<2)++g_autoStateMatches[i];
                    g_autoStateTables[i]=table;
                }
            }
        }
        at=next;
    }
}
static bool compat_discover_state_tables() {
    for(u32 i=0;i<13;++i){g_autoStateMatches[i]=0;g_autoStateTables[i]=0;}
    for(u32 i=0;i<21;++i)g_autoStateLocations[i]={};
    g_autoCandidateRows=0;
    if(!compat_memory_range(0,0x40,false) || *(u16*)g_gameBase!=0x5A4D)return false;
    u32 peoff=*(u32*)(g_gameBase+0x3C);
    if(!compat_memory_range(peoff,24,false))return false;
    const u8* nt=g_gameBase+peoff;
    if(*(const u32*)nt!=0x4550 || *(const u16*)(nt+4)!=0x8664)return false;
    u32 sections=*(const u16*)(nt+6),optionalSize=*(const u16*)(nt+20);
    if(!sections || sections>96 || optionalSize<0x70 ||
       !compat_memory_range(peoff+24,optionalSize,false) ||
       *(const u16*)(nt+24)!=0x20B)return false;
    u64 table=(u64)peoff+24+optionalSize;
    if(!compat_memory_range(table,(u64)sections*40,false))return false;
    for(u32 i=0;i<sections;++i) {
        const u8* section=g_gameBase+table+(u64)i*40;
        u64 rva=*(const u32*)(section+12),length=*(const u32*)(section+8);
        if(!length)length=*(const u32*)(section+16);
        u32 flags=*(const u32*)(section+36);
        // Vtables in the verified builds live in initialized, non-code data.
        if(!(flags&0x40000000u) || !(flags&0x40u) || (flags&0x20000000u) ||
           !length || rva>=g_gameSize)continue;
        if(length>(u64)g_gameSize-rva)length=(u64)g_gameSize-rva;
        compat_scan_data_span(rva,length);
    }
    bool complete=true;
    for(u32 i=0;i<13;++i)if(g_autoStateMatches[i]!=1)complete=false;
    for(u32 i=0;i<13;++i) {
        TextBuf b{};tb_str(b,"R9Q AUTO state=");tb_str(b,kCompatStateModels[i].name);
        tb_str(b," matches=");tb_u64(b,g_autoStateMatches[i]);tb_str(b," tableRVA=0x");tb_hex(b,g_autoStateTables[i]);
        tb_endline(b);append_log(b.bytes,b.len);
    }
    if(!complete)return false;
    for(u32 i=0;i<13;++i) {
        const auto& model=kCompatStateModels[i];u64 tableRva=g_autoStateTables[i];
        if(model.enter_index>=0) {
            u64 slot=tableRva+7*8;
            g_autoStateLocations[model.enter_index]={slot,*(const u64*)(g_gameBase+slot)-(u64)g_gameBase};
        }
        if(model.exit_index>=0) {
            u64 slot=tableRva+8*8;
            g_autoStateLocations[model.exit_index]={slot,*(const u64*)(g_gameBase+slot)-(u64)g_gameBase};
        }
    }
    for(u32 i=0;i<21;++i) {
        const auto& location=g_autoStateLocations[i];
        if(!location.slot || (location.slot&7) || !compat_memory_range(location.slot,8,false) ||
           !compat_memory_range(location.function,1,true))return false;
        for(u32 j=0;j<i;++j)if(g_autoStateLocations[j].slot==location.slot)return false;
    }
    return true;
}
static bool compat_locations_match(const CompatStateLocation* locations) {
    for(u32 i=0;i<21;++i) {
        const auto& p=locations[i];
        if(!p.slot || (p.slot&7) || !compat_memory_range(p.slot,8,false) ||
           !compat_memory_range(p.function,1,true) ||
           *(const u64*)(g_gameBase+p.slot)!=(u64)g_gameBase+p.function)return false;
    }
    return true;
}
static bool compat_is_supplied108() {
    if(g_gameSize!=kCompat108_image_size || !compat_memory_range(0,0x40,false) ||
       *(const u16*)g_gameBase!=0x5A4D)return false;
    u32 offset=*(const u32*)(g_gameBase+0x3C);
    if(!compat_memory_range(offset,24,false) || *(const u32*)(g_gameBase+offset)!=0x4550 ||
       *(const u16*)(g_gameBase+offset+4)!=0x8664 ||
       *(const u32*)(g_gameBase+offset+8)!=kCompat108_timestamp)return false;
    if(!compat_code_matches(kCompat108_flush_rva,kQuadFlushSignature,16) ||
       !compat_memory_range(kCompat108_post_event_rva,64,true) ||
       !v8_post_anchor_matches(g_gameBase+kCompat108_post_event_rva) ||
       !compat_any_pattern((u64)g_gameBase+kCompat108_flush_rva,kCompatAudio_flush) ||
       !compat_any_pattern((u64)g_gameBase+kCompat108_post_event_rva,kCompatAudio_post) ||
       !compat_locations_match(kCompat108States))return false;
    for(u32 i=0;i<13;++i) {
        const auto& model=kCompatStateModels[i];u32 index=(u32)model.enter_index;
        u64 table=kCompat108States[index].slot-7*8;
        if(!compat_memory_range(table,9*8,false) ||
           !compat_model_matches((const u64*)(g_gameBase+table),model))return false;
    }
    return true;
}
static void compat_resolve_targets() {
    g_selectedStateLocations=nullptr;g_compatTargetMode=COMPAT_NONE;
    compat_detect_layout();
    if(!g_cfg.forceAutoScan) {
        if(g_compatLayoutKnown) {
            CompatStateLocation old[21]{};
            for(u32 i=0;i<21;++i)old[i]={g_compat107Layout?kStateHookTargets[i].slot107:kStateHookTargets[i].slot106,
                                                   g_compat107Layout?kStateHookTargets[i].function107:kStateHookTargets[i].function106};
            if(compat_locations_match(old)) {
                g_compatTargetMode=g_compat107Layout?COMPAT_107:COMPAT_106;
                log_simple("R9Q COMPAT verified legacy profile selected");return;
            }
        }
        if(compat_is_supplied108()) {
            g_selectedStateLocations=kCompat108States;g_compatLayoutKnown=true;g_compat107Layout=false;
            g_compatTargetMode=COMPAT_108;g_cfg.flushRva=kCompat108_flush_rva;g_v8PostEventRva=kCompat108_post_event_rva;
            log_simple("R9Q COMPAT verified supplied TU 1.08 profile selected; state targets=21/21");return;
        }
    }
    g_compatLayoutKnown=false;g_compat107Layout=false;
    if(!g_cfg.autoDetectTargets && !g_cfg.forceAutoScan) {
        log_simple("R9Q AUTO disabled; no matching verified profile");return;
    }
    log_simple("R9Q AUTO scanning executable audio anchors and initialized state tables");
    u64 flush=find_unique_exact_rva(kQuadFlushSignature,16);
    u64 post=find_unique_v8_post_event_rva();
    if(!flush || !post ||
       !compat_any_pattern((u64)g_gameBase+flush,kCompatAudio_flush) ||
       !compat_any_pattern((u64)g_gameBase+post,kCompatAudio_post) ||
       !compat_discover_state_tables()) {
        log_simple("R9Q AUTO incomplete or ambiguous; no discovered state targets published");return;
    }
    if(!compat_locations_match(g_autoStateLocations)) {
        log_simple("R9Q AUTO state pointers changed during discovery; rejected");return;
    }
    g_cfg.flushRva=flush;g_v8PostEventRva=post;g_selectedStateLocations=g_autoStateLocations;
    g_compatLayoutKnown=true;g_compatTargetMode=COMPAT_AUTO;
    log_simple("R9Q AUTO validated state targets=21/21; audio anchors=2/2; profile addresses bypassed");
}
