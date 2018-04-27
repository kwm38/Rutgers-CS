#lang racket
(define my-sum (lambda(l)
  (cond
    ((null? l) 0)

    ((number? (car l)) (+ (car l) (my-sum (cdr l))))

    (else
      (my-sum (cdr l))))))
