# Binary diff

L'idea è quella di creare una specie di diff per file binari. In particolare i dettagli
tecnici sono i seguenti: supponiamo di avere due file (A e B) di lunghezze diverse che 
sono "simili", ovvero tali che buona parte dei loro dati è in comune.


Dobbiamo allora cercare di descrivere il contenuto di B in termini di:

* Pezzi presi dal contenuto di A (li supponiamo adiacenti)

* Nuovi dati che non riusciamo a trovare in A


Da ciò generiamo il file diff, che sarà approssimativamente fatto nel modo seguente: in 
testa avremo qualche byte per il numero di versione (visto che probabilmente il formato
cambierà in versioni successive del programma), e per la lunghezza totale del file B. A
seguire ci sarà un alternarsi delle due forme

* "Prendi i dati da A a partire dall'indirizzo ADDR per una lunghezza LEN"

* "Usa i seguenti LEN nuovi dati: DATA"

tutte in ordine di come vanno applicate. È molto semplice scrivere un programma che dato
A ed il file di patch generi B, mentre non penso ci siano metodi ovvi di sviluppare un
algoritmo che dati due file A e B genera il file di patch "più corto" (Ovviamente ci
accontenteremo di qualcosa di subottimale). In particolare bisognerà pensare con una certa
premura a quanto lunghi (in byte) scrivere le dimensioni LEN e gli indirizzi ADDR visto
che influiscono su quale sia la strategia ottimale da adottare. (Ad esempio non conviene
riferirsi al contenuto di A per singoli byte, visto che scrivere questo sul nostro file
di patch ci costa molto di più di un byte).


# TODO-LIST

Attualmente il progetto è in fase di sviluppo. In particolare sono da svolgere i seguenti
compiti "a breve termine":

* Pensare una prima versione dell'algoritmo che permetta di generare i file di patch

* Implementarlo con un linguaggio sensato (che temo dovrà essere il C)

Chiunque abbia delle idee in proposito è ben accetto.

