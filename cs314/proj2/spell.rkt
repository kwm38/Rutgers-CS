; *********************************************
; *  314 Principles of Programming Languages  *
; *  Spring 2018                              *
; *  Student Version                          *
; *********************************************
#lang racket
;; contains "ctv", "A", and "reduce" definitions
(require "include.rkt")

;; contains dictionary definition
;;(require "test-dictionary.rkt")
(require "dictionary.rkt")

;; -----------------------------------------------------
;; HELPER FUNCTIONS

;; *** CODE FOR ANY HELPER FUNCTION GOES HERE ***

;;spell checker takes a bit vector and compares the hashed values of the word to the bit vector
;;if we find that there is a single hashed value for the word that is not in the bitvector, the word is spelt wrong 
(define spell-checker
  (lambda (bitvector hashedWord)
     (if (not (null? hashedWord))
              (if (member (car hashedWord) bitvector)
              (spell-checker bitvector (cdr hashedWord)) #f)
         #t)))

;;hash a word and return a list of hashed values 
(define hashTheWord
  (lambda (hashList word)
    (if (not (null? hashList))
        (cons ((car hashList) word) (hashTheWord (cdr hashList) word))
        '())
    ))

;;append a list of every hashed value in the dictionary
(define hashTheDic
  (lambda (hashList dict)
   (if (not (null? dict))
        (if (not (null? hashList))
            (append (hashTheWord hashList (car dict)) (hashTheDic hashList (cdr dict)))
            '())
        '())
    ))

;; a function used to help reduce the key 
(define keyHelper
  (lambda (letter remainreduction)
    (+ (* remainreduction 31)  (ctv letter))
    )
  )
;; -----------------------------------------------------
;; KEY FUNCTION

(define key
  (lambda (w)
      (reduce keyHelper w 0)
))

;; -----------------------------------------------------
;; HASH FUNCTION GENERATORS

;; value of parameter "size" should be a prime number
(define gen-hash-division-method
  (lambda (size) ;; range of values: 0..size-1
     (lambda (w)
       (modulo (key (reverse w)) size)
)))

;; value of parameter "size" is not critical
;; Note: hash functions may return integer values in "real"
;;       format, e.g., 17.0 for 17

(define gen-hash-multiplication-method
  (lambda (size) ;; range of values: 0..size-1
    (lambda (w)
      (floor (* size (- (* (key (reverse w)) A) (floor (* (key (reverse w)) A))))))
))


;; -----------------------------------------------------
;; EXAMPLE HASH FUNCTIONS AND HASH FUNCTION LISTS

(define hash-1 (gen-hash-division-method 198491329))
(define hash-2 (gen-hash-division-method 217645199))
(define hash-3 (gen-hash-multiplication-method 275604541))
(define hash-4 (gen-hash-multiplication-method 756065179))

(define hashfl-1 (list hash-1 hash-2 hash-3 hash-4))
(define hashfl-2 (list hash-1 hash-3))


;; -----------------------------------------------------
;; EXAMPLE HASH VALUES
;;   to test your hash function implementation
;;
;;  (hash-1 '(h e l l o)) ==> 674
;;  (hash-1 '(d a y))     ==> 395
;;  (hash-1 '(c l a s s)) ==> 360
;;
;;  (hash-2 '(h e l l o)) ==> 139
;;  (hash-2 '(d a y))     ==> 304
;;  (hash-2 '(c l a s s)) ==> 205
;;
;;  (hash-3 '(h e l l o)) ==> 552.0
;;  (hash-3 '(d a y))     ==> 501.0
;;  (hash-3 '(c l a s s)) ==> 247.0
;;
;;  (hash-4 '(h e l l o)) ==> 710.0
;;  (hash-4 '(d a y))     ==> 644.0
;;  (hash-4 '(c l a s s)) ==> 317.0


;; -----------------------------------------------------
;; SPELL CHECKER GENERATOR

;;takes a list of hash functions and a dictionary
;;sends a flattened, dupe-free bit vector and a hashed word to spell checker
(define gen-checker
  (lambda (hashfunctionlist dict)
    (lambda (word)
      (if (not (null? word))
     (spell-checker (remove-duplicates (hashTheDic hashfunctionlist dict)) (remove-duplicates(hashTheWord hashfunctionlist word)))
     #f)
)))


;; -----------------------------------------------------
;; EXAMPLE SPELL CHECKERS

(define checker-1 (gen-checker hashfl-1 dictionary))
(define checker-2 (gen-checker hashfl-2 dictionary))

;; EXAMPLE APPLICATIONS OF A SPELL CHECKER
;;
;;  (checker-1 '(a r g g g)) ==> #f
;;  (checker-2 '(h e l l o)) ==> #t

