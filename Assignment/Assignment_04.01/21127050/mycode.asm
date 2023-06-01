.model small
.stack 100

.data
    str DB 100 dup('$')
    prompt_msg DB 'Nhap chuoi: $'
    output_msg DB 10,13, 'Chuoi da nhap la: $'
    blank_space DB ' $'

.code
    main proc
        mov ax, @data
        mov ds, ax
        
        ; In prompt message
        mov ah, 09h
        mov dx, offset prompt_msg                                                                                      zg
        int 21h
        
        ; Nhap chuoi
        mov si, offset str
        mov cx, 0

    input_loop:
        mov ah, 00h ; nhap mot ki tu khong hien thi len man hinh
        int 16h
        mov dl, al ; luu ki tu vua nhap vao thanh ghi DL
        cmp dl, 0Dh ; kiem tra neu ki tu vua nhap la Enter
        je end_input
        mov [si], dl ; luu ki tu vua nhap vao mang
        inc si
        inc cx
        
        ; In ky tu vua nhap len man hinh
        mov ah, 02h ; dich chuyen con tro den vi tri tiep theo
        mov dl, al ; lay ki tu vua nhap tu thanh ghi DL
        int 21h
        
        jmp input_loop

    end_input:
        mov byte ptr [si], 0 ; them ky tu ket thuc chuoi
        
        ; In output message
        mov ah, 09h
        mov dx, offset output_msg
        int 21h
        
        ; In chuoi da nhap
        mov ah, 0Eh ; ng?t in ký t? c?a máy IBM
        mov si, offset str ; dia chi cua chuoi     
        
    print_loop:
        lodsb ; n?p m?t byte t? DS:[SI] vào AL và tang SI lên 1
        cmp al, 0 ; kiem tra neu ki tu la null
        je end_print
        int 10h ; in ky tu
        jmp print_loop
        
    end_print:
        ; In ky tu xuong dong
        mov ah, 02h ; dich chuyen con tro den vi tri tiep theo
        mov dl, 0Dh ; ky tu xuong dong
        int 21h
        
        ; In ky tu xuong dong
        mov ah, 02h ; dich chuyen con tro den vi tri tiep theo
        mov dl, 0Ah ; ky tu xuong dong
        int 21h
        
        ; Ket thuc chuong trinh
        mov ah, 4ch
        int 21h
    main endp

end main
