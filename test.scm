;; title:  game title
;; author: game developer
;; desc:   short description
;; script: guile

(use-modules (srfi srfi-28))

(define c 2)
(define (TIC)
  (if (btnp 0)
			   (set! c (+ c 1)))
  (if (btnp 1)
			   (set! c (- c 1)))
  (cls c)
		(rect 0 0 240 8 0)
		(print (string-append
		        "cls(" (format "~a" c)
										") -- Use Up/Down to change")))

;; <TILES>
;; 001:efffffffff222222f8888888f8222222f8fffffff8ff0ffff8ff0ffff8ff0fff
;; 002:fffffeee2222ffee88880fee22280feefff80fff0ff80f0f0ff80f0f0ff80f0f
;; 003:efffffffff222222f8888888f8222222f8fffffff8fffffff8ff0ffff8ff0fff
;; 004:fffffeee2222ffee88880fee22280feefff80ffffff80f0f0ff80f0f0ff80f0f
;; 017:f8fffffff8888888f888f888f8888ffff8888888f2222222ff000fffefffffef
;; 018:fff800ff88880ffef8880fee88880fee88880fee2222ffee000ffeeeffffeeee
;; 019:f8fffffff8888888f888f888f8888ffff8888888f2222222ff000fffefffffef
;; 020:fff800ff88880ffef8880fee88880fee88880fee2222ffee000ffeeeffffeeee
;; </TILES>

;; <WAVES>
;; 000:00000000ffffffff00000000ffffffff
;; 001:0123456789abcdeffedcba9876543210
;; 002:0123456789abcdef0123456789abcdef
;; </WAVES>

;; <SFX>
;; 000:000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000304000000000
;; </SFX>

;; <PALETTE>
;; 000:140c1c44243430346d4e4a4e854c30346524d04648757161597dced27d2c8595a16daa2cd2aa996dc2cadad45edeeed6
;; </PALETTE>

