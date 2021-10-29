.text:00566C10 seventh_function_read_serial_45B:       ; CODE XREF: process_acq_start+1BF↑j
.text:00566C10                 push    64h ; 'd'       ; Size
.text:00566C12                 push    0               ; Val
.text:00566C14                 lea     ecx, [ebp+var_7C]
.text:00566C17                 push    ecx             ; void *
.text:00566C18                 call    memset
.text:00566C1D                 add     esp, 0Ch
.text:00566C20                 mov     edx, [ebp+nNumberOfBytesToRead]
.text:00566C23                 push    edx             ; nNumberOfBytesToRead
.text:00566C24                 lea     eax, [ebp+var_7C]
.text:00566C27                 push    eax             ; lpBuffer
.text:00566C28                 mov     ecx, [ebp+var_14]
.text:00566C2B                 mov     edx, [ecx+4]
.text:00566C2E                 push    edx             ; hFile
.text:00566C2F                 call    read_data_serial ; read single byte
.text:00566C34                 add     esp, 0Ch
.text:00566C37                 mov     eax, [ebp+Buffer]
.text:00566C3A                 and     eax, 0FFh
.text:00566C3F                 cmp     eax, 0A0h ; ' '
.text:00566C44                 jz      short eight_function ; always jump because A0 and B0 should not appear in the next 45bytes, it is handled somwhere else
.text:00566C46                 mov     ecx, [ebp+Buffer] ; never executed
.text:00566C49                 and     ecx, 0FFh
.text:00566C4F                 cmp     ecx, 0B0h ; '°'
.text:00566C55                 jnz     short loc_566CB0
.text:00566C57
.text:00566C57 eight_function:                         ; CODE XREF: process_acq_start+202↑j
.text:00566C57                 mov     edx, [ebp+var_14]
.text:00566C5A                 add     edx, 0Ch
.text:00566C5D                 push    edx             ; lpCriticalSection
.text:00566C5E                 call    ds:EnterCriticalSection
.text:00566C64                 mov     [ebp+var_88], 0
.text:00566C6E                 jmp     short nineth_function_loop_over_45B ; loop over 45 bytes
.text:00566C70 ; ---------------------------------------------------------------------------
.text:00566C70
.text:00566C70 increment_counter:                      ; CODE XREF: process_acq_start+25D↓j
.text:00566C70                 mov     eax, [ebp+var_88]
.text:00566C76                 add     eax, 1          ; next byte i=i+1
.text:00566C79                 mov     [ebp+var_88], eax
.text:00566C7F
.text:00566C7F nineth_function_loop_over_45B:          ; CODE XREF: process_acq_start+22C↑j
.text:00566C7F                 mov     ecx, [ebp+var_88] ; loop over 45 bytes
.text:00566C85                 cmp     ecx, [ebp+nNumberOfBytesToRead]
.text:00566C88                 jge     short read_all_45_back_to_beginning ; ; jump once ecx arrives at 2Dh (45)
.text:00566C8A                 mov     edx, [ebp+var_88]
.text:00566C90                 lea     eax, [ebp+edx+var_7C] ; ptr data samples
.text:00566C94                 push    eax             ; load to stack
.text:00566C95                 mov     ecx, offset unk_828C58
.text:00566C9A                 call    fift_funnction_process_byte
.text:00566C9F                 jmp     short increment_counter
.text:00566CA1 ; ---------------------------------------------------------------------------
.text:00566CA1
.text:00566CA1 read_all_45_back_to_beginning:          ; CODE XREF: process_acq_start+246↑j
.text:00566CA1                 mov     ecx, [ebp+var_14]
.text:00566CA4                 add     ecx, 0Ch
.text:00566CA7                 push    ecx             ; lpCriticalSection
.text:00566CA8                 call    ds:LeaveCriticalSection
.text:00566CAE                 jmp     short end_loop
.text:00566CB0 ; ---------------------------------------------------------------------------


.text:00566E00 ; Attributes: bp-based frame
.text:00566E00
.text:00566E00 fift_funnction_process_byte proc near
.text:00566E00
.text:00566E00 var_4= dword ptr -4
.text:00566E00 arg_0= dword ptr  8
.text:00566E00
.text:00566E00 push    ebp
.text:00566E01 mov     ebp, esp
.text:00566E03 push    ecx
.text:00566E04 mov     [ebp+var_4], ecx
.text:00566E07 mov     eax, [ebp+arg_0] ; load the same data as in previous function
.text:00566E0A push    eax
.text:00566E0B mov     ecx, [ebp+var_4]
.text:00566E0E call    decode_bytes
.text:00566E13 mov     esp, ebp
.text:00566E15 pop     ebp
.text:00566E16 retn    4
.text:00566E16 fift_funnction_process_byte endp
.text:00566E16


.text:00566F90 decode_bytes proc near                  ; CODE XREF: fift_funnction_process_byte+E↑p
.text:00566F90
.text:00566F90 var_10= dword ptr -10h
.text:00566F90 var_C= dword ptr -0Ch
.text:00566F90 var_8= dword ptr -8
.text:00566F90 var_4= dword ptr -4
.text:00566F90 arg_0= dword ptr  8
.text:00566F90
.text:00566F90 push    ebp
.text:00566F91 mov     ebp, esp
.text:00566F93 sub     esp, 10h
.text:00566F96 mov     [ebp+var_4], ecx
.text:00566F99 mov     ecx, [ebp+var_4]
.text:00566F9C call    not_relevant_to_decode
.text:00566FA1 and     eax, 0FFh                       ; clear eax
.text:00566FA6 test    eax, eax
.text:00566FA8 jnz     short not_used_1                ; never jump
.text:00566FAA mov     eax, [ebp+var_4]
.text:00566FAD mov     ecx, [ebp+var_4]
.text:00566FB0 mov     edx, [eax+1Ch]                  ; loop some data 0ADh 0BAh 0Dh 0F0h 0ADh 0BAh 0Dh 0F0h 0ADh 0BAh
.text:00566FB3 cmp     edx, [ecx+18h]
.text:00566FB6 jnz     short first_proc_step           ; always jump
.text:00566FB8
.text:00566FB8 not_used_1:                             ; CODE XREF: decode_bytes+18↑j
.text:00566FB8 mov     ecx, [ebp+var_4]
.text:00566FBB call    process_load_data
.text:00566FC0 mov     eax, [ebp+arg_0]
.text:00566FC3 push    eax                             ; int
.text:00566FC4 mov     ecx, [ebp+var_4]
.text:00566FC7 mov     edx, [ecx+1Ch]
.text:00566FCA mov     [ebp+var_8], edx
.text:00566FCD mov     eax, [ebp+var_8]
.text:00566FD0 push    eax                             ; void *
.text:00566FD1 mov     ecx, [ebp+var_4]
.text:00566FD4 mov     edx, [ecx+1Ch]
.text:00566FD7 add     edx, 1
.text:00566FDA mov     eax, [ebp+var_4]
.text:00566FDD mov     [eax+1Ch], edx
.text:00566FE0 mov     ecx, [ebp+var_4]
.text:00566FE3 call    third_proc_step
.text:00566FE8 jmp     short increment_counter_ecx
.text:00566FEA ; ---------------------------------------------------------------------------
.text:00566FEA
.text:00566FEA first_proc_step:                        ; CODE XREF: decode_bytes+26↑j
.text:00566FEA mov     ecx, [ebp+var_4]
.text:00566FED mov     edx, [ecx+1Ch]
.text:00566FF0 add     edx, 1
.text:00566FF3 mov     eax, [ebp+var_4]
.text:00566FF6 cmp     edx, [eax+18h]
.text:00566FF9 jnz     short second_proc_step          ; when edx arrives at ABh jump to print values whe ECX arrives from 0 to 828C58
.text:00566FFB mov     ecx, [ebp+arg_0]                ; print several sample
.text:00566FFE push    ecx                             ; int
.text:00566FFF mov     edx, [ebp+var_4]
.text:00567002 mov     eax, [edx+1Ch]
.text:00567005 mov     [ebp+var_C], eax
.text:00567008 mov     ecx, [ebp+var_C]
.text:0056700B push    ecx                             ; void *
.text:0056700C mov     edx, [ebp+var_4]
.text:0056700F mov     eax, [edx+1Ch]
.text:00567012 add     eax, 1
.text:00567015 mov     ecx, [ebp+var_4]
.text:00567018 mov     [ecx+1Ch], eax
.text:0056701B mov     ecx, [ebp+var_4]
.text:0056701E call    third_proc_step
.text:00567023 mov     ecx, [ebp+var_4]
.text:00567026 call    process_load_data
.text:0056702B jmp     short increment_counter_ecx
.text:0056702D ; ---------------------------------------------------------------------------
.text:0056702D
.text:0056702D second_proc_step:                       ; CODE XREF: decode_bytes+69↑j
.text:0056702D mov     edx, [ebp+arg_0]
.text:00567030 push    edx                             ; int
.text:00567031 mov     eax, [ebp+var_4]
.text:00567034 mov     ecx, [eax+1Ch]
.text:00567037 mov     [ebp+var_10], ecx
.text:0056703A mov     edx, [ebp+var_10]
.text:0056703D push    edx                             ; void *
.text:0056703E mov     eax, [ebp+var_4]
.text:00567041 mov     ecx, [eax+1Ch]
.text:00567044 add     ecx, 1
.text:00567047 mov     edx, [ebp+var_4]
.text:0056704A mov     [edx+1Ch], ecx
.text:0056704D mov     ecx, [ebp+var_4]
.text:00567050 call    third_proc_step
.text:00567055
.text:00567055 increment_counter_ecx:                  ; CODE XREF: decode_bytes+58↑j
.text:00567055                                         ; decode_bytes+9B↑j
.text:00567055 mov     eax, [ebp+var_4]
.text:00567058 mov     ecx, [eax+2Ch]
.text:0056705B add     ecx, 1                          ; ecx goes from 0 to 828C58 then show values
.text:0056705E mov     edx, [ebp+var_4]
.text:00567061 mov     [edx+2Ch], ecx
.text:00567064 mov     esp, ebp
.text:00567066 pop     ebp
.text:00567067 retn    4
.text:00567067 decode_bytes endp

.text:005670A0 ; =============== S U B R O U T I N E =======================================
.text:005670A0
.text:005670A0 ; Attributes: bp-based frame
.text:005670A0
.text:005670A0 process_load_data proc near             ; CODE XREF: decode_bytes+2B↑p
.text:005670A0                                         ; decode_bytes+96↑p
.text:005670A0
.text:005670A0 var_54= dword ptr -54h
.text:005670A0 var_50= byte ptr -50h
.text:005670A0 var_40= byte ptr -40h
.text:005670A0 var_30= byte ptr -30h
.text:005670A0 var_20= byte ptr -20h
.text:005670A0 var_10= dword ptr -10h
.text:005670A0 var_C= dword ptr -0Ch
.text:005670A0 var_8= dword ptr -8
.text:005670A0 var_4= dword ptr -4
.text:005670A0
.text:005670A0 push    ebp
.text:005670A1 mov     ebp, esp
.text:005670A3 sub     esp, 54h
.text:005670A6 mov     [ebp+var_54], ecx
.text:005670A9 push    0
.text:005670AB push    1000h
.text:005670B0 mov     ecx, [ebp+var_54]
.text:005670B3 call    ?allocate@?$allocator@D@std@@QAEPADIPBX@Z ; std::allocator<char>::allocate(uint,void const *)
.text:005670B8 mov     [ebp+var_4], eax
.text:005670BB mov     ecx, [ebp+var_54]
.text:005670BE call    not_relevant_to_decode
.text:005670C3 and     eax, 0FFh
.text:005670C8 test    eax, eax
.text:005670CA jz      p1_proc                         ; always jump
.text:005670D0 mov     eax, [ebp+var_54]               ; never executed
...

.text:00567167 ; ---------------------------------------------------------------------------
.text:00567167
.text:00567167 p1_proc:                                ; CODE XREF: process_load_data+2A↑j
.text:00567167 mov     edx, [ebp+var_54]
.text:0056716A mov     eax, [edx+28h]
.text:0056716D mov     ecx, [ebp+var_54]
.text:00567170 mov     edx, [ecx+24h]
.text:00567173 lea     eax, [edx+eax*4-4]
.text:00567177 mov     ecx, [ebp+var_54]
.text:0056717A cmp     [ecx+20h], eax
.text:0056717D jnb     short no_delay_detected         ; always jump if no delay in communication
.text:0056717F mov     edx, [ebp+var_4]                ; this part of the code is executed if there is a com delay
.text:00567182 push    edx
.text:00567183 mov     eax, [ebp+var_54]
.text:00567186 mov     ecx, [eax+20h]
.text:00567189 add     ecx, 4
.text:0056718C mov     edx, [ebp+var_54]
.text:0056718F mov     [edx+20h], ecx
.text:00567192 mov     eax, [ebp+var_54]
.text:00567195 mov     ecx, [eax+20h]
.text:00567198 push    ecx
.text:00567199 mov     ecx, [ebp+var_54]
.text:0056719C call    sub_5674D0
.text:005671A1 mov     edx, [ebp+var_54]
.text:005671A4 mov     eax, [edx+20h]
.text:005671A7 push    eax
.text:005671A8 mov     ecx, [ebp+var_4]
.text:005671AB push    ecx
.text:005671AC lea     ecx, [ebp+var_30]
.text:005671AF call    sub_567550
.text:005671B4 mov     edx, [ebp+var_54]
.text:005671B7 add     edx, 14h
.text:005671BA mov     ecx, [eax]
.text:005671BC mov     [edx], ecx
.text:005671BE mov     ecx, [eax+4]
.text:005671C1 mov     [edx+4], ecx
.text:005671C4 mov     ecx, [eax+8]
.text:005671C7 mov     [edx+8], ecx
.text:005671CA mov     eax, [eax+0Ch]
.text:005671CD mov     [edx+0Ch], eax
.text:005671D0 jmp     loc_567272
.text:005671D5 ; ---------------------------------------------------------------------------
.text:005671D5
.text:005671D5 no_delay_detected:                      ; CODE XREF: process_load_data+DD↑j
.text:005671D5 mov     ecx, [ebp+var_54]
.text:005671D8 mov     edx, [ebp+var_54]
.text:005671DB mov     eax, [ecx+20h]
.text:005671DE sub     eax, [edx+10h]
.text:005671E1 sar     eax, 2
.text:005671E4 add     eax, 1
.text:005671E7 mov     [ebp+var_C], eax
.text:005671EA mov     ecx, [ebp+var_C]
.text:005671ED shl     ecx, 1
.text:005671EF push    ecx
.text:005671F0 mov     ecx, [ebp+var_54]
.text:005671F3 call    function_not_identified
.text:005671F8 mov     [ebp+var_10], eax
.text:005671FB mov     edx, [ebp+var_4]
.text:005671FE push    edx
.text:005671FF mov     eax, [ebp+var_C]
.text:00567202 mov     ecx, [ebp+var_10]
.text:00567205 lea     edx, [ecx+eax*4]
.text:00567208 push    edx
.text:00567209 mov     ecx, [ebp+var_54]
.text:0056720C call    sub_5674D0
.text:00567211 mov     eax, [ebp+var_10]
.text:00567214 push    eax
.text:00567215 mov     ecx, [ebp+var_54]
.text:00567218 mov     edx, [ecx+0Ch]
.text:0056721B push    edx
.text:0056721C lea     ecx, [ebp+var_40]
.text:0056721F call    sub_567550
.text:00567224 mov     ecx, [ebp+var_54]
.text:00567227 add     ecx, 4
.text:0056722A mov     edx, [eax]
.text:0056722C mov     [ecx], edx
.text:0056722E mov     edx, [eax+4]
.text:00567231 mov     [ecx+4], edx
.text:00567234 mov     edx, [eax+8]
.text:00567237 mov     [ecx+8], edx
.text:0056723A mov     eax, [eax+0Ch]
.text:0056723D mov     [ecx+0Ch], eax
.text:00567240 mov     ecx, [ebp+var_C]
.text:00567243 mov     edx, [ebp+var_10]
.text:00567246 lea     eax, [edx+ecx*4]
.text:00567249 push    eax
.text:0056724A mov     ecx, [ebp+var_4]
.text:0056724D push    ecx
.text:0056724E lea     ecx, [ebp+var_50]
.text:00567251 call    sub_567550
.text:00567256 mov     edx, [ebp+var_54]
.text:00567259 add     edx, 14h
.text:0056725C mov     ecx, [eax]
.text:0056725E mov     [edx], ecx
.text:00567260 mov     ecx, [eax+4]
.text:00567263 mov     [edx+4], ecx
.text:00567266 mov     ecx, [eax+8]
.text:00567269 mov     [edx+8], ecx
.text:0056726C mov     eax, [eax+0Ch]
.text:0056726F mov     [edx+0Ch], eax
.text:00567272
.text:00567272 loc_567272:                             ; CODE XREF: process_load_data+C2↑j
.text:00567272                                         ; process_load_data+130↑j
.text:00567272 mov     esp, ebp
.text:00567274 pop     ebp
.text:00567275 retn
.text:00567275 process_load_data endp

.text:00567450 ; =============== S U B R O U T I N E =======================================
.text:00567450
.text:00567450 ; Attributes: bp-based frame
.text:00567450
.text:00567450 function_not_identified proc near       ; CODE XREF: process_load_data+153↑p
.text:00567450
.text:00567450 var_8= dword ptr -8
.text:00567450 var_4= dword ptr -4
.text:00567450 arg_0= dword ptr  8
.text:00567450
.text:00567450 push    ebp
.text:00567451 mov     ebp, esp
.text:00567453 sub     esp, 8
.text:00567456 mov     [ebp+var_8], ecx
.text:00567459 mov     eax, [ebp+arg_0]
.text:0056745C shl     eax, 2
.text:0056745F push    eax
.text:00567460 mov     ecx, [ebp+var_8]
.text:00567463 call    allocate_space
.text:00567468 mov     [ebp+var_4], eax
.text:0056746B mov     ecx, [ebp+arg_0]
.text:0056746E shr     ecx, 2
.text:00567471 mov     edx, [ebp+var_4]
.text:00567474 lea     eax, [edx+ecx*4]
.text:00567477 push    eax
.text:00567478 mov     ecx, [ebp+var_8]
.text:0056747B mov     edx, [ecx+20h]
.text:0056747E add     edx, 4
.text:00567481 push    edx
.text:00567482 mov     eax, [ebp+var_8]
.text:00567485 mov     ecx, [eax+10h]
.text:00567488 push    ecx
.text:00567489 call    function_nd
.text:0056748E add     esp, 0Ch
.text:00567491 mov     edx, [ebp+var_8]
.text:00567494 mov     eax, [edx+28h]
.text:00567497 push    eax                             ; int
.text:00567498 mov     ecx, [ebp+var_8]
.text:0056749B mov     edx, [ecx+24h]
.text:0056749E push    edx                             ; void *
.text:0056749F mov     ecx, [ebp+var_8]
.text:005674A2 call    unknown_libname_65              ; Microsoft VisualC 2-14/net runtime
.text:005674A7 mov     eax, [ebp+var_8]
.text:005674AA mov     ecx, [ebp+var_4]
.text:005674AD mov     [eax+24h], ecx
.text:005674B0 mov     edx, [ebp+var_8]
.text:005674B3 mov     eax, [ebp+arg_0]
.text:005674B6 mov     [edx+28h], eax
.text:005674B9 mov     ecx, [ebp+arg_0]
.text:005674BC shr     ecx, 2
.text:005674BF mov     edx, [ebp+var_4]
.text:005674C2 lea     eax, [edx+ecx*4]
.text:005674C5 mov     esp, ebp
.text:005674C7 pop     ebp
.text:005674C8 retn    4
.text:005674C8 function_not_identified endp


.text:00567570 ; =============== S U B R O U T I N E =======================================
.text:00567570
.text:00567570 ; Attributes: bp-based frame
.text:00567570
.text:00567570 allocate_space proc near                ; CODE XREF: sub_567420+14↑p
.text:00567570                                         ; function_not_identified+13↑p
.text:00567570
.text:00567570 var_4= dword ptr -4
.text:00567570 arg_0= dword ptr  8
.text:00567570
.text:00567570 push    ebp
.text:00567571 mov     ebp, esp
.text:00567573 push    ecx
.text:00567574 mov     [ebp+var_4], ecx
.text:00567577 push    0                               ; char *
.text:00567579 mov     eax, [ebp+arg_0]
.text:0056757C push    eax                             ; unsigned int
.text:0056757D call    ?_Allocate@std@@YAPADHPAD@Z     ; std::_Allocate(int,char *)
.text:00567582 add     esp, 8
.text:00567585 mov     esp, ebp
.text:00567587 pop     ebp
.text:00567588 retn    4
.text:00567588 allocate_space endp

.text:00567640 ; =============== S U B R O U T I N E =======================================
.text:00567640
.text:00567640 ; Attributes: bp-based frame
.text:00567640
.text:00567640 function_nd proc near                   ; CODE XREF: function_not_identified+39↑p
.text:00567640
.text:00567640 arg_0= dword ptr  8
.text:00567640 arg_4= dword ptr  0Ch
.text:00567640 arg_8= dword ptr  10h
.text:00567640
.text:00567640 push    ebp
.text:00567641 mov     ebp, esp
.text:00567643 jmp     short loc_567657
.text:00567645 ; ---------------------------------------------------------------------------
.text:00567645
.text:00567645 loc_567645:                             ; CODE XREF: function_nd+29↓j
.text:00567645 mov     eax, [ebp+arg_8]
.text:00567648 add     eax, 4
.text:0056764B mov     [ebp+arg_8], eax
.text:0056764E mov     ecx, [ebp+arg_0]
.text:00567651 add     ecx, 4
.text:00567654 mov     [ebp+arg_0], ecx
.text:00567657
.text:00567657 loc_567657:                             ; CODE XREF: function_nd+3↑j
.text:00567657 mov     edx, [ebp+arg_0]
.text:0056765A cmp     edx, [ebp+arg_4]
.text:0056765D jz      short loc_56766B
.text:0056765F mov     eax, [ebp+arg_8]
.text:00567662 mov     ecx, [ebp+arg_0]
.text:00567665 mov     edx, [ecx]
.text:00567667 mov     [eax], edx
.text:00567669 jmp     short loc_567645
.text:0056766B ; ---------------------------------------------------------------------------
.text:0056766B
.text:0056766B loc_56766B:                             ; CODE XREF: function_nd+1D↑j
.text:0056766B mov     eax, [ebp+arg_8]
.text:0056766E pop     ebp
.text:0056766F retn
.text:0056766F function_nd endp
.text:0056766F
.text:00567670 ; [0000001E BYTES: COLLAPSED FUNCTION std::_Allocate(int,char *). PRESS CTRL-NUMPAD+ TO EXPAND]
.text:0056768E align 10h
.text:00567690 ; [0000001F BYTES: COLLAPSED FUNCTION _AFX_OLE_STATE::_AFX_OLE_STATE(void). PRESS CTRL-NUMPAD+ TO EXPAND]
.text:005676AF ; [0000001C BYTES: COLLAPSED FUNCTION _AFX_OLE_STATE::~_AFX_OLE_STATE(void). PRESS CTRL-NUMPAD+ TO EXPAND]
.text:005676CB ; [00000028 BYTES: COLLAPSED FUNCTION sub_5676CB. PRESS CTRL-NUMPAD+ TO EXPAND]
.text:005676F3 align 10h
.text:00567700