#Q = {0,fa1,fa1c,fa2,fb1,fb1c,fb2,check_right,rej,acc,mvl,mvlc,mvl_rej,mvl_acc,rej1,rej2,rej3,rej4,rej5,rej6,acc1,acc2,acc3,acc4,acc5,halt}
#S = {a,b,c}
#G = {a,b,c,_,t,r,u,e,f,l,s,@}
#q0 = 0
#B = _
#F = {acc}
#N = 1

; c-suffix means at the right side of the first c

0 a @ r fa1
0 b @ r fb1
0 c @ r check_right
0 _ _ * rej


fa1 a a r fa1
fa1 b b r fa1
fa1 c c r fa1c
; @ should not appear here
fa1 _ _ * rej

fb1 a a r fb1
fb1 b b r fb1
fb1 c c r fb1c
; @ should not appear here
fb1 _ _ * rej

fa1c a @ * mvlc
fa1c b b * rej
fa1c c c * rej
fa1c _ _ * rej
fa1c @ @ r fa1c

fb1c a a * rej
fb1c b @ * mvlc
fb1c c c * rej
fb1c _ _ * rej
fb1c @ @ r fb1c

; mvl at the right of c, skip @
mvlc a a l mvlc
mvlc b b l mvlc
mvlc c c l mvl
mvlc @ @ l mvlc

;mvl at the left of c, do not skip @
mvl a a l mvl
mvl b b l mvl
mvl c c l mvl
mvl @ @ r 0
; should not reach _, for mvl must follow a @-op at left

; left all examined, should be all _s or @s rightside
check_right a a * rej
check_right b b * rej
check_right c c * rej
check_right @ @ r check_right
check_right _ _ * acc

; reject, find rightmost first and then go back
rej a a r rej
rej b b r rej
rej c c r rej
rej _ _ l mvl_rej

; accept, find rightmost first and then go back
acc a a r acc
acc b b r acc
acc c c r acc
acc _ _ l mvl_acc

; clear when go back (should start at the rightmost)
mvl_rej a _ l mvl_rej
mvl_rej b _ l mvl_rej
mvl_rej c _ l mvl_rej
mvl_rej @ _ l mvl_rej
mvl_rej _ _ r rej1

; clear when go back (should start at the rightmost)
mvl_acc a _ l mvl_acc
mvl_acc b _ l mvl_acc
mvl_acc c _ l mvl_acc
mvl_acc @ _ l mvl_acc
mvl_acc _ _ r acc1

rej1 * f r rej2
rej2 * a r rej3
rej3 * l r rej4
rej4 * s r rej5
rej5 * e r rej6
rej6 * * l halt

acc1 * t r acc2
acc2 * r r acc3
acc3 * u r acc4
acc4 * e r acc5
acc5 * * l halt