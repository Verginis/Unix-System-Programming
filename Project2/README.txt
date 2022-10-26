# Προγραμματισμός Συστήματος - 2η Εργασία

Ονοματεπώνυμο: Βεργίνης Αριστοτέλης    


Οδηγίες Χρήσης:
###############
Για μεταγλώττιση: make (πρέπει να κάνουμε δύο make ένα στο φάκελο server και ένα στον φάκελο client.)

Για εκτέλεση: Για την εκτέλεση του server γράφουμε ./dataServer και μετά δίνουμε τις παραμέτρους. (π.χ  ./dataServer -p 50000 -s 5 -q 10 -b 8092 )
              Για την εκτέλεση του client γράφουμε ./remoteClient και μετά δίνουμε τις παραμέτρους. (π.χ  ./remoteClient -i 127.0.0.1 -p 50000 -d data )

Για διαγραφή εκτελέσιμων: make clean (αντιστοιχα σε κάθε φάκελο.)

Ιδιαιτερότητες Υλοποίησης: 
##########################
Το πρόγραμμα υλοποιεί όλα τα ζητούμενα της εκφώνησης.
Έχουν χρησιμοποιηθεί mutexes, ώστε να επιτευχθεί σωστή επικοινωνία μεταξύ workers και client.



Πρωτόκολλο επικοινωνίας:
#########################
Αρχικά, ο client συνδέεται με το main thread του server, το οποίο δημιουργεί
το welcoming thread. Το welcoming thread επικοινωνεί με τον client και του
στέλνει το block size. Έπειτα, ο client στέλνει πίσω το ονομα του directory, το 
οποίο θέλουμε να επεξεργαστούμε. Το thread παίρνει όλα τα αρχεία που βρίσκονται
μέσα στο συγκεκριμένο directory, και αμέσως μετά ενημερώνουμε τον client πόσα
αρχεία να περιμένει να του επιστραφούν. Έπειτα, τοποθετούμε κάθε αρχείο στην ουρά
και οι worker threads αναλαμβάνουν να στείλουν τις πληροφορίες που χρειάζεται ο client. Αρχικά,
στέλνουμε το ονομα του αρχείου, μετά όλα τα meta data(struct stat), μετά τον αριθμό των block
που θα στείλουμε και τέλος το μέγεθος του τελευταίου block, γιατί μπορεί να μην διαρείται ακριβώς 
και να έχει περισσέψει μνήμη. Έπειτα, εφόσον ο client έχει λάβει αυτές τις πληροφορίες του στέλνουμε
ένα ένα τα block και αυτός τα γράφει στον δίσκο. Τέλος, περιμένουμε ο client να στείλει επιβεβαίωση 
οτι έλαβε όλα τα block που περίμενε και τότε κλείνουμε το socket.


Λειτουργικότητα: 
################


        CLIENT

-------remoteClient.cpp--------


void perror_exit(string message)
Συνάρτηση που εμφανίζει μήνυμα λάθους και τερματίζει το πρόγραμμα.

static bool mkdir_recursive(string dir, bool filename_postfix = true)
Αυτη η συνάρτηση φτιάχνει τους φακελους ενος path.



-------Transfer.cpp--------

write_all(int fd, void *buff, size_t size)
Συνάρτηση που μας  βοηθάει να γράψουμε ολη την πληροφορία στο socket

read_all(int fd, void *buff, size_t size)
Συνάρτηση που μας  βοηθάει να διαβάσουμε ολη την πληροφορία από το socket


        SERVER

-------CommunicationThreadArguments.cpp--------
Εδώ έχουμε τις πληροφορίες για το Communication thread


-------Queue.cpp--------
Εδώ υλοποιούμε την ουρά και έχουμε και τις συναρτήσεις void place(QueueItem data) QueueItem obtain() bool isEmpty() bool isFull().
(έχουν χρησιμοποιηθεί conditions και mutexes για την σωστη λειτουργία.)

-------QueueItem.cpp--------
Εδώ έχουμε τις πληροφορίες για κάθε queue item

-------Transfer.cpp--------

write_all(int fd, void *buff, size_t size)
Συνάρτηση που μας  βοηθάει να γράψουμε ολη την πληροφορία στο socket

read_all(int fd, void *buff, size_t size)
Συνάρτηση που μας  βοηθάει να διαβάσουμε ολη την πληροφορία από το socket

-------WorkerThreadArguments.cpp--------
Στην κλάση αυτή έχουμε τις πληροφορίες που χρειαζόμαστε για τα working threads

-------dataServer.cpp--------

void perror_exit(string message)
Συνάρτηση που εμφανίζει μήνυμα λάθους και τερματίζει το πρόγραμμα.

set<string> list(set<string>& history, string name)
Με την συνάρτηση αυτή επιστρέφουμε όλα τα αρχεία που βρίσκονται μέσα σε ένα φάκελο και τους υποφακέλους του.

void* communication_with_client_routine(void* ptr)
Στην συνάρτηση αυτη επικοινωνούμε με τον client παίρονυμε το directory που περιεχει το file
και του επιστρέφουμε το μεγεθος του file.

void* worker_thread_routine(void* ptr)
Στην συνάρτηση αυτή παρουσιάζεται η υλοποίηση των worker threads, δηλαδή στέλνουμε στον client
το ονομα του αρχείου μετά τα metadata μετά τα τον αριθμό των blocks μετά το μέγεθος του τελευταίου block
και τέλος ένα ένα τα block

void signal_handler(int sig)
στην συνάρτηση αυτή διαχειριζόμαστε το signal Ctrl+C




Πηγές:
#######
1. Διαφάνειες Μαθήματος (open dir, read dir, threads, sockets,bounded buffer problem, κ.α.)
2. Βιβλίο (από το Βιβλίο διάβασα για τα bind, listen, accept, connect, καθώς και πήρα μια βοήθεια 
για την υλοποίηση των συναρτήσεων read_all και write_all)
3. http://nion.modprobe.de/blog/archives/357-Recursive-directory-creation.html (βοήθεια για την συνάρτηση mkdir_recursive(string dir, bool filename_postfix = true))
4. https://docs.microsoft.com/en-us/windows/win32/winsock/sockaddr-2
5. https://linux.die.net/man/2/stat
6. http://www.cs.kent.edu/~ruttan/sysprog/lectures/multi-thread/pthread_cond_init.html
7. https://pubs.opengroup.org/onlinepubs/007904975/functions/pthread_cond_broadcast.html
8. https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html
