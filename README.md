# md5-hashfinder
Abschluss-Projekt des Kurses [Programmieren in C++](https://ad-wiki.informatik.uni-freiburg.de/teaching/ProgrammierenCplusplusSS2012) im Sommersemester 2012 an der [Technischen Fakultät](https://www.tf.uni-freiburg.de/) der Universität Freiburg.

**Frei gewähltes Thema:** HashFinder für MD5- and SHA1-Hashs

## Was kann das Programm?
Das Programm findet Zeichenketten um für einen vorgegebenen Hash passende MD5- und SHA1- Kollisionen zu erzeugen. Dem Programm muss gesagt werden welche Kombinationen gegen diesen Hash geprüft werden sollen. Dies geht entweder durch die Angabe der erlaubten Zeichen und einer minimalen und maximalen Länge für die Kombination, oder durch die
Angabe einer Wörterbuch-Datei. Das Programm soll möglichst schnell sein, um 
so viele Buchstabenkombinationen oder Wörterbucheinträge wie möglich in kurzer Zeit zu testen.

## Übersetzen des Programms
Im Verzeichnis mit dem Quellcode muss folgender Befehl ausgeführt werden:
```
make compile-main
```
Danach sollte die Programm-Datei *HashFinderMain* vorhanden sein.

## Parameter der Kommandozeile
```
   -i, --input-file: read words from a dictionary file
   -a, --min-length: minimal length of the generated combinations
                     Default: 8
   -z, --max-length: maximum length of the generated combinations
                     Default: 8
   -c, --characters: chars used to generate combinations
                     Default: abcdefghijklmnopqrstuvwxyz0123456789
   -h, --hash-algo : can either be sha-1 or md5
                     Default: md5
```

## Vorgehensweise beim Entwurf und bei der Programmierung
1. Überlegen, welche Funktionen und welches Klassendesign am meisten Sinn macht,
   gerade auch unter Beachtung der geplanten Multithreading-Unterstützung
2. Verstehen, wie Multithreading am besten funktioniert:
   Ergebnis: Aufteilung der Gesamtmenge an Aufgaben auf die Anzahl der
   physikalisch verfügbaren Prozessoren. Mehr Threads als Prozessoren.
3. Berechnen der Hashes:
  - Finden einer C++-Klasse, die den MD5-Hash einer Zeichenkette berechnet
  - Finden einer C++-Klasse, die den SHA1-Hash einer Zeichenkette berechnet
  - Anpassen der beiden Klassen, sodass deren Benutzung möglichst identisch ist
  - Klassen für die Hash-Algorithmen befinden sich im Ordner "algorithms".
  - Schreiben einer Makefile mit Makefile.inc für den Unterordner "algorithms".
  - Schreiben einer Klasse HashAlgorithm. Deren Erben sind MD5 und SHA1.
  - Tests die sicherstellen, dass der berechnete (MD5/SHA1)-Hash korrekt ist
  - Beseitigen der Lint-Fehler: Der Linter verlangte, anstatt Referenzen Pointer an die Transformations-Funktionen von MD5 zu übergeben, wenn die übergebene Referenz keine const-Variable ist! Dadurch muss man innerhalb der Funktionen auch wieder dereferenzieren. Da diese Funktionen bei der Berechnung der Hashs sehr oft aufgerufen werden, hatte ich die Befürchtung, dass die Performance durch die zusätzliche Dereferenzierung leidet. Anscheinend erzeugt g++ aber für beide Methoden genau denselben Maschinencode, wie in dieser Diskussion nachzulesen ist: http://stackoverflow.com/questions/1650792/c-function-parameters-use-a-reference-or-a-pointer-and-then-dereference
4. Beim Start des Programms muss die Anzahl der physikalischen Prozessoren
   ermittelt werden, diese ist gleichzeitig die Anzahl der gestarteten Threads:  
    - Zuerst Realisierung mittels `std::thread::hardware_concurrency()`
    -- Funktionierte nicht auf bestimmten Linux-Distributionen
    - Durch Funktion `get_nprocs()` ersetzt (aus `<sys/sysinfo.h>`)
5. Aufteilung der Arbeit auf eine maximale Anzahl an Threads
  - Realisierung der Threads mittels std::thread von -c++0x
  - libpthread muss noch gelinkt werden, da std::thread noch pthread benutzt
  - Im Main-Teil des Programms wird ein HashFinder-Objekt erzeugt
  - dann werden die Kommandozeilenargumente and dieses Objekt weitergegeben
  - dann werden die maximale Anzahl an Threads gestartet, sie führen alle
    gleichzeitig die Funktion process des HashFinder-Objekts aus
  - findet einer der Threads die Kollision, wird die Variable _collision
    gesetzt und alle noch laufenden Threads beenden sich selbst
  - wenn alle Threads  beendet sind, wird die Main-Funktion weiter ausgeführt
6. Testen auf fehlerhaften Speicherzugriff und Memoryleaks mittels Valgrind
7. Profiling (und anschließende Optimierung der Berechnungsgeschwindigkeit)
  - Optimiert habe ich nur noch die Tests ;-)
  - In Jenkins wollte der SHA-1 Kombinations-Test nicht funktionieren: Jenkins ist eine 64bit Maschine, damit hatte ich zuvor nicht viel getestet, SVN-Commit "-fix sha-1 combination search on 64bit" behebt die letzten Fehler... um warnings zu behen musste ich inttypes.h inkludieren und die PRIu64 Konstante bei printf verwenden. Außerdem musste ich beim Umwandeln der nicht null-terminierten char-Pointer die Länge an den Konstruktor der std::string Klasse mit-übergeben.

## Wie bin ich auf die Idee gekommen?
Für eine Übung in "Systeme II" habe ich schon ein C++-Programm geschrieben, um den Schlüssel eines MD5-Hashs über das Ausprobieren von allen Kombinationen zu finden. Dort habe ich die Implementierung von OpenSSL genutzt. Bei diesem Projekt, habe ich nicht auf die Implementation von OpenSSL zurückgegriffen, da ich gegen möglichst wenige externe Bibliotheken linken wollte. Somit kann das Programm ohne weitere Voraussetzungen (g++ reicht) auf einem Linux-OS kompiliert werden.

## Beispiel einer Kombinations-Attacke auf einen MD5-Hash:
```
./HashFinderMain -a6 -z7 -chas123 -hmd5 35e5d160921d131d9114f1b4ee5f9d55
[Main] HashFinder version 1.0.
[Main] Hashing-Algorithm: MD5.
[Main] Hash: 35e5d160921d131d9114f1b4ee5f9d55.
[Main] Using combination attack:
       - word length: 6 - 7
       - characters: has123
[Main] Combinations: 326592
[Main] I will start 2 threads now.
[Thread 2] Started...
[Thread 1] Started...
[Thread 1] Tried 147092 strings.
[Thread 1] Stopped after 658648 microseconds.
[Thread 2] Collision found => hash123
[Thread 2] Tried 151711 strings.
[Thread 2] Stopped after 659704 microseconds.
[Main] Regular shutdown.
[Main] Thank you for using this program!
```

## Beispiel einer Kombinations-Attacke auf einen SHA-1-Hash:
```
./HashFinderMain -a6 -z7 -chas123 -hsha1 586b64caacfbdfd67d2a8d323510ed5b72a61e0b
[Main] HashFinder version 1.0.
[Main] Hashing-Algorithm: SHA-1.
[Main] Hash: 586b64caacfbdfd67d2a8d323510ed5b72a61e0b.
[Main] Using combination attack:
       - word length: 6 - 7
       - characters: has123
[Main] Combinations: 326592
[Main] I will start 2 threads now.
[Thread 2] Started...
[Thread 1] Started...
[Thread 1] Tried 147474 strings.
[Thread 1] Stopped after 1124158 microseconds.
[Thread 2] Collision found => hash123
[Thread 2] Tried 151711 strings.
[Thread 2] Stopped after 1124932 microseconds.
[Main] Regular shutdown.
[Main] Thank you for using this program!
```

Bei obigem Beispiel gibt es insgesamt 326592 mögliche Kombinationen.
Dies ergibt sich aus Anzahl der versch. Zeichen ^ Länge des Wort
In diesem Fall also:
```
 6 ^ 6 =  46656
+6 ^ 7 = 279936
       = 326592
```

## Beispiel einer Wörterbuch-Attacke auf einen MD5-Hash:
```
./HashFinderMain -idictionary.txt -hmd5 35e5d160921d131d9114f1b4ee5f9d55
[Main] Reading dictionary entries into memory...done.
[Main] HashFinder version 1.0.
[Main] Hashing-Algorithm: MD5.
[Main] Hash: 35e5d160921d131d9114f1b4ee5f9d55.
[Main] Using dictionary attack: 349902 words
[Main] I will start 2 threads now.
[Thread 1] Started...
[Thread 2] Started...
[Thread 1] Tried 134421 strings.
[Thread 1] Stopped after 514935 microseconds.
[Thread 2] Collision found => hash123
[Thread 2] Tried 134252 strings.
[Thread 2] Stopped after 514530 microseconds.
[Main] Regular shutdown.
[Main] Thank you for using this program!
```

## Beispiel einer Wörterbuch-Attacke auf einen SHA-1-Hash:
```
./HashFinderMain -idictionary.txt -hsha1 586b64caacfbdfd67d2a8d323510ed5b72a61e0b
[Main] Reading dictionary entries into memory...done.
[Main] HashFinder version 1.0.
[Main] Hashing-Algorithm: SHA-1.
[Main] Hash: 586b64caacfbdfd67d2a8d323510ed5b72a61e0b.
[Main] Using dictionary attack: 349902 words
[Main] I will start 2 threads now.
[Thread 1] Started...
[Thread 2] Started...
[Thread 1] Tried 124990 strings.
[Thread 1] Stopped after 1057790 microseconds.
[Thread 2] Collision found => hash123
[Thread 2] Tried 134252 strings.
[Thread 2] Stopped after 1057880 microseconds.
[Main] Regular shutdown.
[Main] Thank you for using this program!
```

Das dem Hash entsprechende Wort steht in Zeile 309204. Wie man sieht hat Thread 1 in der ersten Hälfte der Wörterbuchdatei gesucht und Thread 2 hat in der zweiten Hälfte gesucht.
Thread 1 untersuchte also Wörter 0 - 174951.
Thread 2 untersuchte die Wörter 174952 - 349901.
Thread 2 wurde fündig, da 174952+134252 das gesuchte Wort in Zeile 309204 ist.
Nachdem die Kollision gefunden wurde, hat Thread 1 sich sofort selbst beendet.

## Tests
Bei den Tests war es zunächst wichtig die Hash-Funktionen auf ihre Korrektheit zu prüfen. Es gibt deshalb jeweils einen Test für die beiden Hashing-Algorithmen (MD5 und SHA-1). Diese sind im Unterordner "algorithms" gespeichert und können dort durch ein make test ausgeführt werden.

Für die HashFinder-Klasse und das HashFinderMain-Programm gibt es auch
Tests für alle Möglichkeiten:
* CommandLineArguments
* Wörterbuch auslesen
* MD5-Kollision aus Wörterbuch
* SHA-1-Kollision aus Wörterbuch
* MD5-Kollision aus Kombination mit Länge 4
* SHA-1-Kollision aus Kombination mit Länge 4



