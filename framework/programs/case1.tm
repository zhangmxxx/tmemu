#Q = {0,scan_a,scan_b,rej,acc,mvl,cp,mvl2,csm,mvr,wrt,mvl3,halt,mvr2,mvl4,rej1,rej2,rej3,rej4,rej5,rej6,rej7,rej8,rej9,rej10,rej11,rej12,rej13}
#S = {a,b}
#G = {a,b,c,_}
#q0 = 0
#B = _
#F = {acc}
#N = 2

; format checking, tape 2 stay the same for all cases
0 a* a* r* scan_a ; use 0 * * rej, 0 a a scan_a here
0 b* b* ** rej
0 _* _* ** rej

scan_a a* a* r* scan_a
scan_a b* b* r* scan_b
scan_a _* _* ** rej

scan_b a* a* ** rej
scan_b b* b* r* scan_b
scan_b _* _* ** acc

; write Illegal_Input to tape1, tape2 stay the same
rej ** ** ** mvr2
mvr2 a* a* r* mvr2
mvr2 b* b* r* mvr2
mvr2 _* _* l* mvl4
mvl4 a* _* l* mvl4
mvl4 b* _* l* mvl4
mvl4 _* _* r* rej1
rej1 _* I* r* rej2
rej2 _* l* r* rej3
rej3 _* l* r* rej4
rej4 _* e* r* rej5
rej5 _* g* r* rej6
rej6 _* a* r* rej7
rej7 _* l* r* rej8
rej8 _* _* r* rej9
rej9 _* I* r* rej10
rej10 _* n* r* rej11
rej11 _* p* r* rej12
rej12 _* u* r* rej13
rej13 _* t* ** halt

; copy to tape2 and clear
; move left first
acc _* _* l* mvl
mvl a* a* l* mvl
mvl b* b* l* mvl
mvl _* _* r* cp

; copy
cp a* _a rr cp
cp b* _b rr cp
cp _* _* ll mvl2 ; mvl according to tape2

mvl2 *a *a ll mvl2
mvl2 *b *b ll mvl2
mvl2 *_ *_ rr csm

; consume a on tape2, write c to tape1
; always have b at the right side of a
csm *a *_ *r mvr
csm *b *b l* halt ; set tape1 to last c
mvr *a *a *r mvr
mvr *b *b ** wrt
wrt *b cb rr wrt
wrt *_ *_ *l mvl3 
mvl3 *a *a *l mvl3
mvl3 *b *b *l mvl3
mvl3 *_ *_ *r csm

; extra mvr and mvl with little difference