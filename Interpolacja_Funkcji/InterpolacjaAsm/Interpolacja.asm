.data
jeden qword 1.0	

.code

LicznikLaGrange proc
MOVZX R10, WORD PTR [RSP + 48] ; xCount
MOVZX R11, WORD PTR [RSP + 40] ; Index
mov rcx, 0
LoopBegin:
cmp rcx, r10 ;Sprawdz czy petla juz nie powinna przestac sie krecic (i == xCount)
jz LoopEnd
cmp rcx, r11 ;Sprawdz, czy nie nalezy pominac tej iteracji (i == index)
jz LoopSkip
movsd xmm2, xmm1 ; Przenies xParam z xmm1 do roboczego xmm2
movsd xmm3, qword ptr [r8+rcx*8] ;Przenieœ xCoords[i]
subsd xmm2, xmm3 ; Wykonaj xParam - xCoords[i]
mulsd xmm0, xmm2 ; Domnó¿ wynik poprzedniego dzia³ania do wyniku
inc rcx ;Inkrementuj licznik
jmp LoopBegin
LoopSkip:
inc rcx ;Inkrementuj licznik
jmp LoopBegin
LoopEnd:
movsd qword ptr [r9], xmm0
ret
LicznikLaGrange endp


MianownikLaGrange proc
mov rax, 0
movsd xmm0, [jeden] ;Przygotuj 1 do mno¿enia
LoopBegin:
cmp rax, r9 ;Sprawdz czy petla juz nie powinna przestac sie krecic (i == xCount)
jz LoopEnd
cmp rax, r8 ;Sprawdz, czy nie nalezy pominac tej iteracji (i == index)
jz LoopSkip
movsd xmm1, qword ptr [rcx+r8*8] ;Pobierz xCoords[index] do xmm3
movsd xmm2, qword ptr [rcx+rax*8] ;Pobierz xCoords[i] do xmm2
subsd xmm1, xmm2 ;Wykonaj xCoords[index] - xCoords[i]
mulsd xmm0, xmm1 ;Domnó¿ wynik poprzedniego dzia³ania do wyniku
inc rax ;Inkrementuj licznik
jmp LoopBegin
LoopSkip:
inc rax ;Inkrementuj licznik
jmp LoopBegin
LoopEnd:
movsd qword ptr [rdx], xmm0
ret
MianownikLaGrange endp


FinalLaGrange proc
movups xmm0, [rcx]
movups xmm1, [rdx]
divsd xmm0,xmm1
movups [rcx], xmm0
ret
FinalLaGrange endp

DivideVector proc
movups xmm0, [rcx]
movups xmm1, [rdx]
divpd xmm0,xmm1
movups [rcx], xmm0
ret
DivideVector endp

Aitken proc
movsd xmm4, qword ptr [rsp+40] ;Wyci¹gnij ze stosu xi
movsd xmm5, qword ptr [rsp+48] ;Wyci¹gnij ze stosu xParam
movsd xmm0, xmm3 ;Przenies xj do xmm0
subsd xmm0, xmm4 ;Wykonaj xj - xi z mianownika
subsd xmm3, xmm5 ;Wykonaj xj - xParam
mulsd xmm3, xmm1 ;Wykonaj yi * (xj - xParam)
subsd xmm4, xmm5 ;Wykonaj xi - xParam
mulsd xmm4, xmm2 ;Wykonaj yj * (xi - xParam)
subsd xmm3, xmm4 ;Wykonaj g³ówne odejmowanie z licznika
divsd xmm3, xmm0 ;Podziel licznik przez mianownik
movsd qword ptr [rcx], xmm3
ret
Aitken endp

end