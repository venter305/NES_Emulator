.segment "HEADER"
MAPPER = $0
MIRROR = $0 ; 0 = horizontal 1 = vertical

.byte 'N','E','S',$1A
.byte $02 ; prg banks
.byte $01 ; chr banks
.byte (MAPPER & $0F)*$10 + MIRROR
.byte (MAPPER & $F0)

.segment "CHARS"
.incbin "patternTable1.chr"

.segment "VECTORS"
.word nmi
.word reset
.word irq

;set up character mapping
.repeat 10,i ;numbers
.charmap $30+i,i+1
.endrep
.repeat 26,i ;letters
.charmap $41+i,$B+i
.endrep
.charmap $20,$FF ;space
.charmap $3A,$25 ;colon
.charmap $2F,$27

;1-Byte Variables
RED_EMPHASIS = $0200
GREEN_EMPHASIS = $0201
BLUE_EMPHASIS = $0202
GRAY_EMPHASIS = $0203
ARROW_POS = $0204
BTN_LOCK = $0205
BACKGROUND_COLOR = $0206

.segment "CODE"
loop:
  jmp loop

nmi:

  ldx #$3F
  ldy #$00
  lda BACKGROUND_COLOR
  jsr write_ppu_val

  ;Red Emphasis
  ldx #$20
  ldy #$27
  lda RED_EMPHASIS
  clc
  adc #$01
  jsr write_ppu_val

  ;Green Emphasis
  ldx #$20
  ldy #$47
  lda GREEN_EMPHASIS
  clc
  adc #$01
  jsr write_ppu_val

  ;Blue Emphasis
  ldx #$20
  ldy #$67
  lda BLUE_EMPHASIS
  clc
  adc #$01
  jsr write_ppu_val

  ;Gray Emphasis
  ldx #$20
  ldy #$87
  lda GRAY_EMPHASIS
  clc
  adc #$01
  jsr write_ppu_val

  lda #$0A
  ldx RED_EMPHASIS
  beq :+
  ora #$20
:
  ldx GREEN_EMPHASIS
  beq :+
  ora #$40
:
  ldx BLUE_EMPHASIS
  beq :+
  ora #$80
:
  ldx GRAY_EMPHASIS
  beq :+
  ora #$01
:
  sta $2001

  ldx #$20
  ldy ARROW_POS
  lda #$FF
  jsr write_ppu_val

  ;input
  ;load controller shift register
  lda #$01
  sta $4016
  lda #$00
  sta $4016

  ldx #$08
  lda #$00
  clc
  ;read from controller 1 bit at a time

  lda #$00
  sta $00
:
  asl $00
  lda #$01
  and $4016
  adc $00
  sta $00
  dex
  bne :-

  ;move cursor with dpad
  lda #$08 ;UP
  and $00
  beq :+
  lda BTN_LOCK
  and #$08
  bne up_end
  ;lock BTN
  lda #$08
  ora BTN_LOCK
  sta BTN_LOCK
  ;move cursor
  lda ARROW_POS
  sec
  sbc #$20
  sta ARROW_POS
  lda #$08
  cmp ARROW_POS
  bcc up_end
  lda #$88
  sta ARROW_POS
  jmp up_end
:
  ;clear lock for btn
  lda #$F7
  and BTN_LOCK
  sta BTN_LOCK
up_end:

  lda #$04 ;Down
  and $00
  beq :+
  lda BTN_LOCK
  and #$04
  bne down_end
  ;lock BTN
  lda #$04
  ora BTN_LOCK
  sta BTN_LOCK
  ;move cursor
  lda #$20
  clc
  adc ARROW_POS
  sta ARROW_POS
  lda #$88
  cmp ARROW_POS
  bcs down_end
  lda #$28
  sta ARROW_POS
  jmp down_end
:
  ;clear lock for btn
  lda #$FB
  and BTN_LOCK
  sta BTN_LOCK
down_end:

  lda #$80 ; A
  and $00
  beq released_A
  lda BTN_LOCK
  and #$80
  bne end_A
  ;lock BTN
  lda #$80
  ora BTN_LOCK
  sta BTN_LOCK
  ;press btn
  lda #$28
  cmp ARROW_POS
  bne :+
  lda #$01
  eor RED_EMPHASIS
  and #$01
  sta RED_EMPHASIS
  jmp end_A
:
  lda #$48
  cmp ARROW_POS
  bne :+
  lda #$01
  eor GREEN_EMPHASIS
  and #$01
  sta GREEN_EMPHASIS
  jmp end_A
:
  lda #$68
  cmp ARROW_POS
  bne :+
  lda #$01
  eor BLUE_EMPHASIS
  and #$01
  sta BLUE_EMPHASIS
  jmp end_A
:
  lda #$88
  cmp ARROW_POS
  bne end_A
  lda #$01
  eor GRAY_EMPHASIS
  and #$01
  sta GRAY_EMPHASIS
  jmp end_A
released_A:
  lda #$7F
  and BTN_LOCK
  sta BTN_LOCK
end_A:

  lda #$40 ;B
  and $00
  beq released_B
  lda #$40
  and BTN_LOCK
  bne end_B
  ;lock btn
  lda #$40
  ora BTN_LOCK
  sta BTN_LOCK
  ;press btn
  inc BACKGROUND_COLOR
  lda #$40
  cmp BACKGROUND_COLOR
  bne end_B
  lda #$00
  sta BACKGROUND_COLOR
released_B:
  lda #$BF
  and BTN_LOCK
  sta BTN_LOCK
end_B:

  lda #$10 ;start
  and $00
  beq released_Start
  lda #$10
  and BTN_LOCK
  bne end_Start
  ;lock BTN
  lda #$10
  ora BTN_LOCK
  sta BTN_LOCK
  ;press button
  lda #$00
  sta RED_EMPHASIS
  sta GREEN_EMPHASIS
  sta BLUE_EMPHASIS
  lda #$30
  sta BACKGROUND_COLOR
  lda #$28
  sta ARROW_POS
  ldx #$20
  ldy #$48
  lda #$FF
  jsr write_ppu_val
  ldy #$68
  jsr write_ppu_val
released_Start:
  lda #$EF
  and BTN_LOCK
  sta BTN_LOCK
end_Start:

  ldx #$20
  ldy ARROW_POS
  lda #$26
  jsr write_ppu_val

  ;Reset view
  lda #$00
  sta $2006
  sta $2006

  rti

reset:
  sei

  ; clear nametable
  lda #$20
  sta $2006
  lda #$00
  sta $2006
  lda #$00
  jsr clear_nametable

  ;Pallets
  ldx #$3F
  ldy #$00
  lda #$30
  jsr write_ppu_val
  ldy #$03
  lda #$0F
  jsr write_ppu_val

  lda #$20
  sta $2006
  lda #$21
  sta $2006

  jsr display_text
  .asciiz "RED:"
  lda #$20
  sta $2006
  lda #$41
  sta $2006

  jsr display_text
  .asciiz "GREEN:"

  lda #$20
  sta $2006
  lda #$61
  sta $2006

  jsr display_text
  .asciiz "BLUE:"

  lda #$20
  sta $2006
  lda #$81
  sta $2006

  jsr display_text
  .asciiz "GRAY:"

  lda #$20
  sta $2006
  lda #$C1
  sta $2006
  jsr display_text
  .asciiz "UP/DOWN: MOVE CURSOR"

  lda #$20
  sta $2006
  lda #$E1
  sta $2006
  jsr display_text
  .asciiz "A: TOGGLE COLOR EMPHASIS"

  lda #$21
  sta $2006
  lda #$01
  sta $2006
  jsr display_text
  .asciiz "B: CHANGE BACKGROUND COLOR"

  lda #$21
  sta $2006
  lda #$21
  sta $2006
  jsr display_text
  .asciiz "START: RESET"

  ;initalize variables
  lda #$00
  sta RED_EMPHASIS
  sta GREEN_EMPHASIS
  sta BLUE_EMPHASIS
  sta GRAY_EMPHASIS

  lda #$28
  sta ARROW_POS

  lda #$00
  sta BTN_LOCK

  lda #$30
  sta BACKGROUND_COLOR

  ;address to start rendering at
  lda #$00
  sta $2006
  sta $2006

  ;enable rendering
  lda #$0A
  sta $2001

  ;enable vblank nmi
  lda #$80
  sta $2000


  jmp loop

irq:
  rti

;arguments:
; x = Address Hi byte
; y = Address Lo byte
; a = Value
write_ppu_val:
  stx $2006
  sty $2006
  sta $2007
  rts

;arguments:
; a = Clear Value
clear_nametable:
  sta $00 ;Save Clear Value
  txa
  pha
  tya
  pha

  ldx #$20 ; columns
  ldy #$1E ; rows
  lda $00  ; Set Stored Clear value
:
  sta $2007
  dex
  bne :-
  ldx #$20
  dey
  bne :-

  pla
  tay
  pla
  tax
  rts

;arguments:
; x = Size of string
; data stored after jsr instruction
display_text:
  pla
  sta $00 ; lo byte of the return Address
  pla
  sta $01 ; hi byte of the return address
  ;add 1 to return address
  inc $00
  bcc :+
  inc $01
:

  ;display text
  ldy #$00
:
  lda ($00),y
  sta $2007
  iny
  cmp #$00
  bne :-

  ;return to the address after the text data
  clc
  tya
  adc $00
  bcc :+
  inc $01
:
  sta $00
  jmp ($00)
