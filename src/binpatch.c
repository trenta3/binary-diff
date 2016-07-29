#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>

#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "debug.h"

#define false 0
#define true 1

// Dichiariamo alcuni tipi che ci saranno comodi
typedef unsigned long int ulint;

// Uso: binpatch fileA difffile fileB
int main (int argc, char *argv[]) {
	FILE *fileB;
	int fdA = 0, fdD = 0, status = 0;
	size_t Asize, Dsize;
	struct stat Astat, Dstat;
	const char *Amap, *Dmap;

	// Stampiamo il numero di versione
	printf("*** binpatch v. 001 - A minimal binary patcher ***\n");

	// Vediamo se ci hanno passato il numero giusto di argomenti
	check (argc < 4, "\nVanno passati almeno tre argomenti.\nUso: binpatch fileA difffile fileB\nDato il file delle differenze tra A e B produce B.");

	// Apriamo i tre file controllando che non ci siano errori
	fdA = open(argv[1], O_RDONLY);
	check (fdA < 0, "open %s RDONLY failed", argv[1]);
	on_exit(lambda (void, (int s __attribute__ ((unused)), void* arg), { close((int)(intptr_t)arg); }), (void*)(intptr_t)fdA);

	status = fstat(fdA, &Astat);
	check (status < 0, "stat %s failed", argv[1]);

	Asize = (size_t) Astat.st_size;
	Amap = mmap(0, Asize, PROT_READ, MAP_SHARED, fdA, 0);
	check (Amap == MAP_FAILED, "mmap %s failed", argv[1]);

	fdD = open(argv[2], O_RDONLY);
	check (fdD < 0, "open %s RDONLY failed", argv[2]);
	on_exit(lambda (void, (int s __attribute__ ((unused)), void* arg), { close((int)(intptr_t)arg); }), (void*)(intptr_t)fdD);

	status = fstat(fdD, &Dstat);
	check (status < 0, "stat %s failed", argv[2]);

	Dsize = (size_t) Dstat.st_size;
	Dmap = mmap(0, Dsize, PROT_READ, MAP_SHARED, fdD, 0);
	check (Dmap == MAP_FAILED, "mmap %s failed", argv[2]);

	fileB = fopen(argv[3], "wb");
	check (fileB == NULL, "fopen %s failed", argv[3]);
	on_exit(lambda (void, (int s __attribute__ ((unused)), void* arg), { fclose((int)(intptr_t)arg); }), (void*)(intptr_t)fileB);

	// --- FINE DEI CONTROLLI --- INIZIA IL CODICE VERO ---
	log_info("File aperti. Inizio della applicazione patch.");
	// Controlliamo la versione del file di patch (con che cosa inizia)
	check (strncmp("BDIFv", Dmap, 5) != 0, "file %s is not a bdif patch file", argv[2]);

	if (strncmp("001", &Dmap[5], 3) == 0) {
		// La versione del file di patch è la prima, procediamo con la lettura
		log_info("Riconosciuta versione 001 del file di patch");
		// Iniziamo da dopo l'header e procediamo processando tutto il file
		ulint posD = 8;
		for ( ; posD < Dsize; ) {
			// Ogni volta leggiamo il valore (ed il MSB) e decidiamo cosa fare.
			uint length;
			memcpy(&length, &Dmap[posD], sizeof(ulint));
			// Facciamo avanzare il puntatore della posizione, visto che ormai l'abbiamo letto
			posD += sizeof(ulint);
			if (length == (length | Tilde(Tilde0UL >> 1))) {
				// Se rimane uguale quando setto il MSB allora avevo MSB=1
				length &= Tilde0UL >> 1;
				// Adesso quindi leggiamo tutto il pezzo di A corrispondente e lo sbattiamo in fileB
				ulint ptpos;
				memcpy(&ptpos, &Dmap[posD], sizeof(ulint));
				check (fwrite(&Amap[ptpos], length, 1, fileB) != 1, "fwrite failed to fileB");
				posD += sizeof(ulint);
			} else {
				// Altrimenti avevo MSB=0 e quindi pesco i dati grezzi direttamente dal diff
				check (fwrite(&Dmap[posD], length, 1, fileB) != 1, "fwrite failed to fileB");
				posD += length;
			}
		}
	} else check (true, "unrecognized bdif version of patch file %s", argv[2]);

	log_info("File chiusi. Programma terminato");
	return 0;
}
