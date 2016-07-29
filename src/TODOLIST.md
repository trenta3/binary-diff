Da fare:

* Aggiungere le chiusure munmap

* Eliminare le fopen, etc. e sostituirle con chiamate di sistema Linux

* Aggiungere i suggerimenti al kernel su come usiamo la mmap (sequenziale)

* Scriverla architecture independent (attualmente ci basiamo sul conoscere la dimensione di un po' di cose)
