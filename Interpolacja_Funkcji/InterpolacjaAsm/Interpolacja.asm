.code


LicznikLaGrange proc
pop r10w ; index
pop r10w 
pop r10w 
pop r10w 
pop r10w 
pop r10w 
pop r10w 
pop r10w 
pop r10w 
pop r10w 
pop r10w 
pop r10w 
pop r10w 
pop r10w 
pop r10w 
pop r10w 

pop r11w ; xCount
mov rcx, 0
LoopBegin:
cmp rcx, r11 ;Sprawdz czy petla juz nie powinna przestac sie krecic
jz LoopEnd
cmp rcx, r10 ;Sprawdz, czy nie nalezy pominac tej iteracji
jz LoopSkip
movsd xmm2, xmm1 ; Przenies xParam z xmm1 do roboczego xmm2
subsd xmm2, mmword ptr [r8+rax*8] ; Wykonaj xParam - xCoords[i]
mulsd xmm0, xmm2 ; Domnó¿ wynik poprzedniego dzia³ania do wyniku
inc rcx ;Inkrementuj licznik
jmp LoopBegin
LoopSkip:
inc rcx
jmp LoopBegin
LoopEnd:
movsd mmword ptr [r9], xmm0
ret
LicznikLaGrange endp

end