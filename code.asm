include G:\masm32\include\masm32rt.inc

.xmm
.const
__@strfmti db 37,100,32,0

__@strfmtiln db 37,100,32,10,0

.code
__@function0:
enter 12, 1
lea eax, dword ptr [ebp - 4]
push eax
push 12345
push 42
pop ebx
pop eax
sub eax, ebx
push eax
pop eax
pop ebx
mov dword ptr [ebx], eax
lea eax, dword ptr [ebp - 8]
push eax
push 10
pop eax
pop ebx
mov dword ptr [ebx], eax
lea eax, dword ptr [ebp - 8]
push dword ptr [eax]
lea eax, dword ptr [ebp - 4]
push dword ptr [eax]
pop ebx
pop eax
cmp eax, ebx
setle al
sub al, 1
movsx eax, al
push eax
pop eax
test eax, eax
jz else_branch1
lea eax, dword ptr [ebp - 8]
push dword ptr [eax]
push offset __@strfmti
call crt_printf
add esp, 8
jmp end_if1
else_branch1:
lea eax, dword ptr [ebp - 4]
push dword ptr [eax]
push offset __@strfmti
call crt_printf
add esp, 8
end_if1:
push 1
push 1
pop ebx
pop eax
cmp eax, ebx
sete al
sub al, 1
movsx eax, al
push eax
pop eax
test eax, eax
jz else_branch2
push 9
push offset __@strfmti
call crt_printf
add esp, 8
jmp end_if2
else_branch2:
push 1
push 1
pop ebx
pop eax
cmp eax, ebx
setne al
sub al, 1
movsx eax, al
push eax
pop eax
test eax, eax
jz else_branch3
push 1
push offset __@strfmti
call crt_printf
add esp, 8
jmp end_if3
else_branch3:
end_if3:
end_if2:
lea eax, dword ptr [ebp - 4]
push eax
push 10
pop eax
pop ebx
mov dword ptr [ebx], eax
lea eax, dword ptr [ebp - 8]
push eax
push 11
pop eax
pop ebx
mov dword ptr [ebx], eax
lea eax, dword ptr [ebp - 4]
push dword ptr [eax]
push 4
pop ebx
pop eax
imul ebx
push eax
push offset __@strfmtiln
call crt_printf
add esp, 8
push 100
push 2
pop ebx
pop eax
cdq 
idiv ebx
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
cmp eax, ebx
setle al
sub al, 1
movsx eax, al
push eax
push offset __@strfmtiln
call crt_printf
add esp, 8
lea eax, dword ptr [ebp - 4]
push dword ptr [eax]
lea eax, dword ptr [ebp - 8]
push dword ptr [eax]
push 1
pop ebx
pop eax
sub eax, ebx
push eax
pop ebx
pop eax
cmp eax, ebx
setne al
sub al, 1
movsx eax, al
push eax
push offset __@strfmtiln
call crt_printf
add esp, 8
push 100
push 1
pop ebx
pop eax
sub eax, ebx
push eax
push 2
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
