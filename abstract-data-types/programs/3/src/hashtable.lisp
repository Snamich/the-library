(defconstant +A+ .6180339887)

(defun make-hashtable (size)
  (let ((hash-size size)
         (current-size 0)
         (hash-table (make-array size :initial-element nil)))
    (labels ((insert (key value) ;; private insert method
               (let ((position (find-pos key)))
                 (if (null (elt hash-table position))
                     (progn
                       (setf (elt hash-table position) (cons key value))
                       (incf current-size)
                       (if (> (/ current-size hash-size) .2)
                           (rehash))
                       t)
                     nil)))
             (find-pos (key) ;; private find position method
               (loop for i = 0 then (+ i 1)
                  for current-position = (hash key i) then (hash key i)
                  until (or (null (elt hash-table current-position))
                            (equal key (car (elt hash-table current-position))))
                  finally (return current-position)))
             (find-key (key)
               (let ((position (find-pos key)))
                 (if (not (null (elt hash-table position)))
                     (elt hash-table position)
                     nil)))
             (rehash () ;; resize the hash table if the load factor is too high
               (let ((old-table (copy-seq hash-table)))
                 (setq hash-size (* hash-size 4))
                 (setq current-size 0)
                 (setq hash-table (make-array hash-size :initial-element nil))
                 (loop for item across old-table
                    if (not (null item))
                    do (insert (car item) (cdr item)))))
             (hash (key i) ;; hashing function for peg boards
               (loop with size = (length key)
                  for item in key
                  summing item into total
                  finally (return (mod (+ (floor (* hash-size (mod (* (* total size) +A+) 1)))
                                          (* i (floor (* (mod (mod (* (* total size) +A+) 1) 1)
                                                         hash-size))))
                                       hash-size)))))
      (list ;; provide public interface
       #'(lambda (key) (find-key key))
       #'(lambda (key value) (insert key value))))))

(defun hash-insert (hashtable key value)
  (funcall (car (cdr hashtable)) key value))

(defun hash-find (hashtable key)
  (funcall (car hashtable) key))
