.386
.MODEL FLAT, C

.data

.CODE
CopyScreenData PROC      ; FirstScreenData, NextScreenData, NextScreenLength   [Pos][Length][Data]
	push ebp 
	mov  ebp, esp
	sub  esp, 20h

	mov  ebx, dword ptr [ebp + 10h]      ;NextScreenLength
	mov  esi, dword ptr [ebp + 0Ch]      ;NextScreenData
	jmp	CopyEnd
CopyNextBlock:
	mov edi, [ebp + 8h]     ;  FirstScreenData
	lodsd					; ��lpNextScreen�ĵ�һ��˫�ֽڣ��ŵ�eax��,����DIB�иı������ƫ��
	add edi, eax			; lpFirstScreenƫ��eax	
	lodsd					; ��lpNextScreen����һ��˫�ֽڣ��ŵ�eax��, ���Ǹı�����Ĵ�С
	mov ecx, eax
	sub ebx, 8				; ebx ��ȥ ����dword
	sub ebx, ecx			; ebx ��ȥDIB���ݵĴ�С
	rep movsb
CopyEnd:
	cmp ebx, 0				; �Ƿ�д���� 
	jnz CopyNextBlock

	add esp, 20h
	pop ebp
	ret
CopyScreenData ENDP
END

