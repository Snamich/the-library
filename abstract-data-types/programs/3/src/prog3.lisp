(defparameter *prog3-table* (make-hashtable 4096))

(defparameter null-peg 0)
(defparameter single-peg 1)
(defparameter double-peg 2)

(defun solve-puzzle (pegs)
  (let ((solution (solve-board pegs (list))))
    (if (null solution)
        (format t "No solution.~%")
        (print-solution solution))))

(defun solve-board (board path)
  "Attempt to find a solution to the given board."
  (let ((board-result (hash-find *prog3-table* board)))
    (cond
      ((not (null board-result)) ;; check if the board has been solved before
       (cdr board-result))
      ((solved? board) path) ;; is the board solved after the previous jump?
      (t (let ((result
                (loop with size = (length board)
                   for item from 0 to (1- size)
                   thereis (multiple-value-bind (num-jumps jump-positions) (peg-can-jump item board size)
                             (if (eq num-jumps 2)
                                 (let* ((first-jump (car jump-positions))
                                        (second-jump (car (cdr jump-positions)))
                                        (first-board (solve-board (jump-peg board item first-jump)
                                                                  (cons (cons item first-jump) path)))
                                        (second-board (solve-board (jump-peg board item second-jump)
                                                                   (cons (cons item second-jump) path))))
                                   (if first-board
                                       first-board
                                       second-board))
                                 (when (eq num-jumps 1)
                                   (solve-board (jump-peg board item (car jump-positions))
                                                (cons (cons item (car jump-positions)) path))))))))
           (hash-insert *prog3-table* board result)
           result)))))

(defun print-solution (solution)
  (let ((path (reverse solution)))
    (loop for item in path
       do (format t "Jump peg ~a to peg ~a~%" (1+ (car item)) (1+ (cdr item))))))

(defun solved? (board)
  (if (null (remove-if #'(lambda (x) (eq x double-peg)) board))
      t
      nil))

(defun jump-peg (board peg pos)
  (let ((new-board (copy-seq board)))
    (setf (elt new-board peg) null-peg
          (elt new-board pos) double-peg)
    (clean-board new-board)))

(defun peg-can-jump (peg board board-size)
  "Looks to the left and right of peg to see if a jump is possible.
Returns the number of jumps possible and position of jumps."
  (if (not (eq (elt board peg) double-peg))
      (let ((num-jumps 0)
            (jump-positions nil))
        (if (and (> peg 1)
                 (eq (elt board (- peg 1)) double-peg)
                 (eq (elt board (- peg 2)) single-peg))
            (progn
              (incf num-jumps)
              (push (- peg 2) jump-positions))
            (if (and (> peg 2)
                     (eq (elt board (- peg 1)) single-peg)
                     (eq (elt board (- peg 2)) single-peg)
                     (eq (elt board (- peg 3)) single-peg))
                (progn
                  (incf num-jumps)
                  (push (- peg 3) jump-positions))))
        (if (and (> (- board-size peg) 2)
                 (eq (elt board (+ peg 1)) double-peg)
                 (eq (elt board (+ peg 2)) single-peg))
            (progn
              (incf num-jumps)
              (push (+ peg 2) jump-positions))
            (if (and (> (- board-size peg) 3)
                     (eq (elt board (+ peg 1)) single-peg)
                     (eq (elt board (+ peg 2)) single-peg)
                     (eq (elt board (+ peg 3)) single-peg))
                (progn
                  (incf num-jumps)
                  (push (+ peg 3) jump-positions))))
        (values num-jumps jump-positions))
      (list 0 nil)))

(defun clean-board (board)
  (remove null-peg board))
