extern common_interrupt_handler

%macro idt_no_error 1

global idt_handler_%1
idt_handler_%1:
   cli

   ; Push error code
   push 0

   ; Push interrupt number
   push %1

   ; Push registers
   push rax
   push rbx
   push rcx
   push rdx
   push rbp
   push rdi
   push rsi
   push r8
   push r9
   push r10
   push r11
   push r12
   push r13
   push r14
   push r15

   xor rax, rax
   mov ax, ds
   push rax

   xor rax, rax
   ; Change Data Segment
   ; Data Segment is selected with 0x10
   mov ax, 0x10
   mov ds, ax
   mov es, ax

   ; Pass in pointer to the registers
   mov rdi, rsp

   call common_interrupt_handler

   ; Change data segment back
   pop rax
   mov ds, ax
   mov es, ax

   pop r15
   pop r14
   pop r13
   pop r12
   pop r11
   pop r10
   pop r9
   pop r8
   pop rsi
   pop rdi
   pop rbp
   pop rdx
   pop rcx
   pop rbx
   pop rax
   
   ; Pop interrupt number
   add rsp, 8

   ; Pop error code
   add rsp, 8
   
   sti

   iretq

%endmacro

%macro idt_error 1

global idt_handler_%1
idt_handler_%1:

    cli
   
   ; Push interrupt number
   push %1

   ; Push registers
   push rax
   push rbx
   push rcx
   push rdx
   push rbp
   push rdi
   push rsi
   push r8
   push r9
   push r10
   push r11
   push r12
   push r13
   push r14
   push r15

   xor rax, rax
   mov ax, ds
   push rax

   ; Change Data Segment
   ; Data Segment is selected with 0x10
   mov ax, 0x10
   mov ds, ax
   mov es, ax

   ; Pass in pointer to the registers
   mov rdi, rsp

   call common_interrupt_handler

   ; Change data segment back
   pop rax
   mov ds, ax
   mov es, ax
   
   pop r15
   pop r14
   pop r13
   pop r12
   pop r11
   pop r10
   pop r9
   pop r8
   pop rsi
   pop rdi
   pop rbp
   pop rdx
   pop rcx
   pop rbx
   pop rax
   
   ; Pop interrupt number
   add rsp, 8

   sti

   iretq

%endmacro

; These are all the handlers
idt_no_error 0
idt_no_error 1
idt_no_error 2
idt_no_error 3
idt_no_error 4
idt_no_error 5
idt_no_error 6
idt_no_error 7
idt_error 8
idt_no_error 9
idt_error 10
idt_error 11
idt_error 12
idt_error 13
idt_error 14
idt_no_error 15
idt_no_error 16
idt_error 17
idt_no_error 18
idt_no_error 19
idt_no_error 20
idt_error 21
idt_no_error 22
idt_no_error 23
idt_no_error 24
idt_no_error 25
idt_no_error 26
idt_no_error 27
idt_no_error 28
idt_error 29
idt_error 30
idt_no_error 31
idt_no_error 32
idt_no_error 33
idt_no_error 34
idt_no_error 35
idt_no_error 36
idt_no_error 37
idt_no_error 38
idt_no_error 39
idt_no_error 40
idt_no_error 41
idt_no_error 42
idt_no_error 43
idt_no_error 44
idt_no_error 45
idt_no_error 46
idt_no_error 47
idt_no_error 48
idt_no_error 49
idt_no_error 50
idt_no_error 51
idt_no_error 52
idt_no_error 53
idt_no_error 54
idt_no_error 55
idt_no_error 56
idt_no_error 57
idt_no_error 58
idt_no_error 59
idt_no_error 60
idt_no_error 61
idt_no_error 62
idt_no_error 63
idt_no_error 64
idt_no_error 65
idt_no_error 66
idt_no_error 67
idt_no_error 68
idt_no_error 69
idt_no_error 70
idt_no_error 71
idt_no_error 72
idt_no_error 73
idt_no_error 74
idt_no_error 75
idt_no_error 76
idt_no_error 77
idt_no_error 78
idt_no_error 79
idt_no_error 80
idt_no_error 81
idt_no_error 82
idt_no_error 83
idt_no_error 84
idt_no_error 85
idt_no_error 86
idt_no_error 87
idt_no_error 88
idt_no_error 89
idt_no_error 90
idt_no_error 91
idt_no_error 92
idt_no_error 93
idt_no_error 94
idt_no_error 95
idt_no_error 96
idt_no_error 97
idt_no_error 98
idt_no_error 99
idt_no_error 100
idt_no_error 101
idt_no_error 102
idt_no_error 103
idt_no_error 104
idt_no_error 105
idt_no_error 106
idt_no_error 107
idt_no_error 108
idt_no_error 109
idt_no_error 110
idt_no_error 111
idt_no_error 112
idt_no_error 113
idt_no_error 114
idt_no_error 115
idt_no_error 116
idt_no_error 117
idt_no_error 118
idt_no_error 119
idt_no_error 120
idt_no_error 121
idt_no_error 122
idt_no_error 123
idt_no_error 124
idt_no_error 125
idt_no_error 126
idt_no_error 127
idt_no_error 128
idt_no_error 129
idt_no_error 130
idt_no_error 131
idt_no_error 132
idt_no_error 133
idt_no_error 134
idt_no_error 135
idt_no_error 136
idt_no_error 137
idt_no_error 138
idt_no_error 139
idt_no_error 140
idt_no_error 141
idt_no_error 142
idt_no_error 143
idt_no_error 144
idt_no_error 145
idt_no_error 146
idt_no_error 147
idt_no_error 148
idt_no_error 149
idt_no_error 150
idt_no_error 151
idt_no_error 152
idt_no_error 153
idt_no_error 154
idt_no_error 155
idt_no_error 156
idt_no_error 157
idt_no_error 158
idt_no_error 159
idt_no_error 160
idt_no_error 161
idt_no_error 162
idt_no_error 163
idt_no_error 164
idt_no_error 165
idt_no_error 166
idt_no_error 167
idt_no_error 168
idt_no_error 169
idt_no_error 170
idt_no_error 171
idt_no_error 172
idt_no_error 173
idt_no_error 174
idt_no_error 175
idt_no_error 176
idt_no_error 177
idt_no_error 178
idt_no_error 179
idt_no_error 180
idt_no_error 181
idt_no_error 182
idt_no_error 183
idt_no_error 184
idt_no_error 185
idt_no_error 186
idt_no_error 187
idt_no_error 188
idt_no_error 189
idt_no_error 190
idt_no_error 191
idt_no_error 192
idt_no_error 193
idt_no_error 194
idt_no_error 195
idt_no_error 196
idt_no_error 197
idt_no_error 198
idt_no_error 199
idt_no_error 200
idt_no_error 201
idt_no_error 202
idt_no_error 203
idt_no_error 204
idt_no_error 205
idt_no_error 206
idt_no_error 207
idt_no_error 208
idt_no_error 209
idt_no_error 210
idt_no_error 211
idt_no_error 212
idt_no_error 213
idt_no_error 214
idt_no_error 215
idt_no_error 216
idt_no_error 217
idt_no_error 218
idt_no_error 219
idt_no_error 220
idt_no_error 221
idt_no_error 222
idt_no_error 223
idt_no_error 224
idt_no_error 225
idt_no_error 226
idt_no_error 227
idt_no_error 228
idt_no_error 229
idt_no_error 230
idt_no_error 231
idt_no_error 232
idt_no_error 233
idt_no_error 234
idt_no_error 235
idt_no_error 236
idt_no_error 237
idt_no_error 238
idt_no_error 239
idt_no_error 240
idt_no_error 241
idt_no_error 242
idt_no_error 243
idt_no_error 244
idt_no_error 245
idt_no_error 246
idt_no_error 247
idt_no_error 248
idt_no_error 249
idt_no_error 250
idt_no_error 251
idt_no_error 252
idt_no_error 253
idt_no_error 254
idt_no_error 255

