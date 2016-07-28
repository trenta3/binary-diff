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


# Piani d'attacco

## Algoritmo stupido

Provo a descrivere in breve il primo algoritmo che mi è venuto in mente. Spero si riesca
a comprendere.

Ci teniamo due puntatori `pa` e `pb` che puntano a dove stiamo gestendo i file A e B. (All'
inizio sono entrambi al byte iniziale, ovvero `0`). Adesso consideriamo il byte `B[pb]` e ci
chiediamo se esista una stringa sufficientemente lunga di caratteri di A che coincida con
una che inizia da `B[pb]` e continua per un po'. (Sufficientemente lunga si intende più di
una lunghezza fissata attorno alla decina di byte).

Se esiste allora scriviamo questa informazione nel file di patch (Basta inserire il byte
di A in cui inizia ed il numero di byte di lunghezza) e muoviamo `pb` alla fine della
corrispondente stringa in B, nel primo carattere che non viene compreso.
Se non esiste allora aggiungiamo il byte `B[pb]` al buffer dei dati nuovi (quelli che non
erano riconoscibili nel file A) e passiamo `pb` al byte successivo. La prima volta che
verrà incontrato un byte esistente nel file A il buffer dei dati nuovi verrà svuotato e
scritto nel file di patch nelle modalità indicate più sopra.

Il modo più banale per controllare se una tale stringa esiste è semplicemente di controllare
tutte le lettere di A e fermarsi sulla prima che coincide con `B[pb]`. Se le successive
lettere coincidono allora espandiamo la stringa, altrimenti ci teniamo da parte un puntatore
a questa e procediamo con il cercare la prossima. In questo modo troviamo la più grande
stringa comune ad A ed al pezzo di B a partire dal byte `pb`.

Ciò ci obbliga purtroppo a ripercorrere tutto il file A (in teoria) per quasi ogni byte di
B (anche se ciò in casi reali non dovrebbe accadere). Molto probabilmente si può provare a
pensare ad adattare questo algoritmo al metodo "divide et impera" sperando di guadagnarne
qualcosa se si effettua la ricerca su meta di tutto il file A per volta per poi "unirle",
un po' come viene fatto nel `merge sort`.
