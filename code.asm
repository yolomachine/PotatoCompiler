include G:\masm32\include\masm32rt.inc

.xmm
.const
__@strfmti db 37,100,32,0

__@strfmtiln db 37,100,32,10,0

.code
__@function0:
enter 8, 1
lea eax, dword ptr [ebp - 4]
push eax
push 12345
pop eax
pop ebx
mov dword ptr [ebx], eax
lea eax, dword ptr [ebp - 8]
push eax
push 10
pop eax
pop ebx
mov dword ptr [ebx], eax
lea eax, dword ptr [ebp - 4]
push dword ptr [eax]
push offset __@strfmti
call crt_printf
add esp, 8
push 100
push offset __@strfmti
call crt_printf
add esp, 8
push 100
push 1
pop ebx
pop eax
sub eax, ebx
push eax
push offset __@strfmtiln
call crt_printf
add esp, 8
push 1
push 2
pop ebx
pop eax
add eax, ebx
push eax
push offset __@strfmtiln
call crt_printf
add esp, 8
lea eax, dword ptr [ebp - 4]
push dword ptr [eax]
lea eax, dword ptr [ebp - 8]
push dword ptr [eax]
pop ebx
pop eax
add eax, ebx
push eax
push offset __@strfmtiln
call crt_printf
add esp, 8
leave
ret 0

start:
call __@function0
exit
end start
