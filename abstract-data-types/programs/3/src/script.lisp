#!/usr/local/bin/sbcl --script
(load "hashtable.lisp")
(load "prog3.lisp")

(defun parse-prog3-inputfile (file)
  (with-open-file (in file :direction :input)
    (let ((input-number (read in nil)))
      (loop for i from 1 to input-number
         collecting (+ (read in nil) 1) into board
         finally (return (reverse board))))))

(let* ((inputfile (car (cdr sb-ext:*posix-argv*)))
       (board (parse-prog3-inputfile inputfile)))
  (solve-puzzle board))
