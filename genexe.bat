@echo off
ml.exe /c /coff code.asm
link.exe /subsystem:console code.obj
code.exe