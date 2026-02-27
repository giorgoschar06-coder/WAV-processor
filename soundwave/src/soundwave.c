#include <stdio.h>
#include <stdlib.h> //Απαραίτητη βιβλιοθήκη για το exit(1); 
#include <string.h> //Απαραίτητη βιβλιοθήκη για το strcmp(,);
#include <limits.h> //Απαραίτητη βιβλιοθήκη για το UINT_MAX
#include <math.h> //Απαραίτητη βιβλιοθήκη για το round();

int readB1counter=0; //Global Μεταβλητή για να ελέγχω πόσα bytes διάβασα συνολικά από το αρχείο  

int readB1(void) //Συνάρτηση στην οποία διαβάζω ένα προς ένα τα byte ενός αρχείου .wav
{                      
    int c=getchar(); //Διαβάζω ένα byte
    if (c==EOF) //Για να ξέρω πότε έχει τελειώσει το αρχείο (Εαν έχει τελειώσει επιστρέφω -1)
        {
            return -1; 
        }
    readB1counter++; //Αυξάνετε κατά ένα κάθε φορά που καλείτε η συνάρτηση readB1
    return c & 0xFF; //Επιστρέφω την τιμή του byte (τιμές από 0-255)
}                    //& 0xFF χρησιμοποιήτε για αποφυγή επέκτασης προσήμου διασφαλίζοντας ότι οι τιμές θα είναι πάντα θετικές

unsigned int read2bytes() //Συνάρτηση στην οποία όταν πρέπει να διαβάσω σε 2 bytes από το header, μετατρέπει τα bytes σε ένα ακέραιο με το πρότυπο little-endian 
{        
      unsigned int byte2=readB1() | (readB1() << 8); //byte χαμηλότερης αξίας + byte μεγαλύτερης αξίας μετατοπισμένο 8 θέσεις αφού 1byte=8bits (το | τα συνδυάζει)
      return byte2;
}

unsigned int read4bytes() //Ίδια συνάρτηση με read2bytes, απλώς σε αυτή χρησιμοποιήτε όταν πρέπει να διαβάσω 4 bytes από το header
{        
      unsigned int byte4=readB1() | (readB1() << 8) | (readB1() << 16) | (readB1() << 24);
      return byte4;
}

void IntToChar(unsigned char *buff, int n) //Μετατροπή των bytes σε χαρακτήρες
{
    for (int k=0;k<n;k++) //Επανάληψη ανάλογα με το πόσα bytes θέλω να μετατρέψω σε χαρακτήρες
    {
        buff[k]=readB1(); //Αποθηκεύονται προσωρινά τα bytes σε χαρακτήρες σε ένα buffer n θέσεων για να ελέξω την ορθότητα τους αργότερα  
    }
}

void RIFF(unsigned char *buff) //Συνάρτηση στην οποία ελέγχω την ορθότητα του "RIFF" στο header του αρχείου
{
    IntToChar(buff, 4); //Μετατρέπω τα πρώτα 4 bytes σε χαρακτήρες 
    if (buff[0]== 'R' && buff[1]=='I' && buff[2]=='F' && buff[3]=='F') //ελέγχω εαν σχηματίζουν την λέξη "RIFF"
        {
        }
    else //αλιώς τερματίζω το πρόγραμμα με error message
        {
            fprintf(stderr, "Error! \"RIFF\" not found\n");
            exit(1);
        }
}

unsigned int Size_Of_File() //Συνάρτηση στην οποία αποθηκεύω το μέγεθος του αρχείου 
{                           //Πραγματικό μέγεθος ενός αρχείου = size+8 (Τα πρώτα 8 byte που διαβάζονται, δεν μετριούνται στο size)
   unsigned int size=read4bytes();
   return size;
}                   

void WAVE(unsigned char *buff) //Συνάρτηση στην οποία ελέγχω την ορθότητα του "WAVE" στο header του αρχείου 
{
    IntToChar(buff, 4);
    if (buff[0]=='W' && buff[1]=='A' && buff[2]=='V' && buff[3]=='E')
    {
    }
    else
        {
            fprintf(stderr, "Error! \"WAVE\" not found\n");
            exit(1);
        }
}

void fmt(unsigned char *buff) //Συνάρτηση στην οποία ελέγχω την ορθότητα του "fmt " στο header του αρχείου
{
    IntToChar(buff, 4);
    if (buff[0]=='f' && buff[1]=='m' && buff[2]=='t' && buff[3]==' ')
    {
    }
    else
        {
            fprintf(stderr, "Error! \"fmt \" not found\n");
            exit(1);
        }
}

unsigned int format_chunk() //Συνάρτηση στην οποία υπολογίζω και ελέγχω την ορθότητα του χώρου που καταλαμβάνουν τα δεδομένα του τμήματος format
{
       unsigned int chunk=read4bytes();
       if (chunk!=16) //Με βάση τις οδηγίες της εργασίας, αυτή η τιμή πρέπει να είναι πάντοτε 16
            {
                printf("size of format chunk: %u\n", chunk);
                fprintf(stderr, "Error! size of format chunk should be 16\n");
                exit(1);
            }
       return chunk;
}

unsigned int WAVE_type() //Συνάρτηση στην οποία υπολογίζω και ελέγχω την ορθότητα του τύπου του WAVE format
{
    unsigned int WAVEt=read2bytes();
    if (WAVEt!=1) //Με βάση τις οδηγίες της εργασίας, αυτή η τιμή πρέπει να είναι πάντοτε 1
        {
            printf("WAVE type format: %u\n", WAVEt);
            fprintf(stderr, "Error! WAVE type format should be 1\n");
            exit(1);
        }
    return WAVEt;
}

unsigned int mono_stereo() //Συνάρτηση στην οποία υπολογίζω και ελέγχω εαν ο ήχος είναι μονοφωνικός ή στερεοφωνικός με δύο κανάλια
{
    unsigned int monost=read2bytes(); 
    if (monost==1)
        {
            return 1;
        }
    else if (monost==2) 
        {
            return 2;
        }
    else //Δεν μπορεί να πάρει άλλη τιμή εκτός από το 1 ή 2
        {
            printf("mono/stereo: %u\n", monost);
            fprintf(stderr, "Error! mono/stereo should be 1 or 2\n");
            exit(1);
        }
    return monost;
}

unsigned int SampleRate() //Συνάρτηση στην οποία αποθηκεύω τον ρυθμό δειγματοληψίας του ήχου
{
    unsigned int sampleR=read4bytes();
    return sampleR;
}

unsigned int BytesPerSec() //Συνάρτηση στην οποία αποθηκεύω το πλήθος των bytes ανά δευτερόλεπτο ήχου, που είναι καταχωρημένα στο αρχείο
{
    unsigned int bytesSec=read4bytes();
    return bytesSec;
}

unsigned int BlockAlign(unsigned int sampleR, unsigned int bytesSec) //Συνάρτηση στην οποία υπολογίζω και ελέγχω την ορθότητα του πλήθους των bytes που απαιτούνται
{                                                                    //για την καταχώρηση της πληροφορίας του ήχου σε μια χρονική στιγμή
    unsigned int blockA=read2bytes(); 
    if (bytesSec==sampleR * blockA) //Πρέπει υποχρεωτικά να ισχύει αυτή η σχέση για να θεωρήτε ορθό το BlockAlign
        {
        }
    else
        {
            printf("block alignment: %u\n", blockA);
            fprintf(stderr, "Error! bytes/second should be sample rate x block alignment\n");
            exit(1);  
        }
    return blockA;
}

unsigned int BitsPerSample(unsigned int monost, unsigned int blockA) //Συνάρτηση στην οποία υπολογίζω και ελέγχω την ορθότητα του πλήθους των bits που απαιτούνται...
{
    unsigned int BPS=read2bytes(); 
    if (BPS==8 || BPS==16) //Πρέπει υποχρεωτικά να έχει τιμή 8 ή 16
        {
        }
    else
        {
            printf("bits/sample: %u\n", BPS);
            fprintf(stderr, "Error! bits/sample should be 8 or 16\n");
            exit(1);  
        }
    if (blockA==(BPS/8) * monost) //Πρέπει επίσης να ισχύει αυτή η σχέση για να θεωρήτε ορθό
        {
        }
    else
        {
            printf("bits/sample: %u\n", BPS);
            fprintf(stderr, "Error! block alignment should be bits per sample / 8 x mono/stereo\n");
            exit(1);  
        }
    return BPS;
}

void DataChunk(unsigned char *buff) //Συνάρτηση στην οποία ελέγχω την ορθότητα του "data" προς στο τέλος του header του αρχείου
{
    IntToChar(buff, 4);
    if (buff[0]=='d' && buff[1]=='a' && buff[2]=='t' && buff[3]=='a')
    {
    }
    else
        {
            fprintf(stderr, "Error! \"data\" not found\n");
            exit(1);
        }
}

unsigned int SizeOfData() //Συνάρτηση στην οποία υπολογίζω και ελέγχω την ορθότητα του χώρου που καταλαμβάνουν τα δεδομένα του τμήματος data που θα ακολουθήσουν
{
    unsigned int Dsize=read4bytes();
   
    if (Dsize==0) //Δεν θεωρείτε ορθό το Data Size ενός αρχείου να είναι 0 (Δεν γίνετε ένα αρχείο ήχου να μην περιέχει καθόλου ήχο)
        {
            printf("size of data chunk: %u\n", Dsize);
            fprintf(stderr, "Error! insufficient data\n");
            exit(1); 
        }  
    return Dsize;
}

void CompareSizes(unsigned int size) //Συνάρτηση στην οποία ελέγχω την περίπτωση που τα δεδομένα ήχου ξεπερνούν το αναμενόμενο size+8 του αρχείου που δόθηκε στο header
{
    if(((unsigned int) readB1counter) > (size+8))
        {
            fprintf(stderr, "Error! bad file size (found data past the expected end of file)\n");
            exit(1); 
        }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int info() //Συνάρτηση στην οποία υλοποιήτε η υποεντολή info σε οποιοδήποτε αρχείο .wav δοθεί και ζητηθεί από τον χρήστη
{
    unsigned char buff[4]; //Πίνακας χαρακτήρων 4ων θέσεων ο οποίος θα χρησιμοποιηθεί για τον έλεγχο ορθότητας του "RIFF", "WAVE", "fmt " και "data" μέσα στο αρχείο

    //Παρακάτω ακολουθεί το κάλεσμα όλων των πιο πάνω συναρτήσεων για τον έλεγχο εγκυρότητας ενός αρχειου .wav
    //και ακολούθως η εκτύπωση όλων των απαραίτητων δεδομένων που απαιτεί μια σωστή info, μαζί με τα κατάλληλα μυνήματα εξόδου
    RIFF(buff);

    unsigned int size=Size_Of_File(); 
    printf("size of file: %u\n", size);

    WAVE(buff);

    fmt(buff);

    unsigned int chunk=format_chunk();
    printf("size of format chunk: %u\n", chunk);

    unsigned int WAVEt=WAVE_type();
    printf("WAVE type format: %u\n", WAVEt);

    unsigned int monost=mono_stereo();
    if (monost==1)
        {
            printf("mono/stereo: 1\n");
        }
    else if (monost==2)
        {
            printf("mono/stereo: 2\n");
        }

    unsigned int sampleR=SampleRate();
    printf("sample rate: %u\n", sampleR);

    unsigned int bytesSec=BytesPerSec();
    printf("bytes/sec: %u\n", bytesSec); 

    unsigned int blockA=BlockAlign(sampleR, bytesSec);
    printf("block alignment: %u\n", blockA);

    unsigned int BPS=BitsPerSample(monost, blockA);
    printf("bits/sample: %u\n", BPS);
    
    DataChunk(buff);

    unsigned int Dsize=SizeOfData();
    int a; //Βοηθητική μεταβλητή για το διάβασμα των byte μετά το header 
    while ((a=readB1())!=-1) //Επανάληψη για το δίαβασμα των SampleData-OtherData(εαν υπάρχουν) του αρχείου μέχρι να τελειώσει, δηλαδή το readB1 να επιστρέψει -1 στο a
        {
        }
    if(((unsigned int) readB1counter) < (size+8)) //Έλεγχος εαν τα byte που διαβαστήκαν είναι λιγότερα από αυτά που αναμένονταν με βάση το size+8 του header
        {
            printf("size of data chunk: %u\n", Dsize);
            fprintf(stderr, "Error! insufficient data\n");
            exit(1);    
        }
        printf("size of data chunk: %u\n", Dsize);
     
    CompareSizes(size);
    //Εαν όλα τα δεδομένα του αρχείου πληρούν τις προδιαγραφές τότε τυπώνονται κανονικά όλα τα μυνήματα μαζί με τα δεδομένα χωρίς κανένα μήνυμα σφάλματος
    return 0; //Με το τέλος της υποεντολής info επιστρέφω 0 πράγμα που σημαίνει το αρχείο ήχου έτρεξε την υποεντολή επιτυχώς χωρίς σφάλματα
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int rate(double fp_rate) //Συνάρτηση στην οποία υλοποιήτε η υποεντολή rate με οποιαδήποτε έγκυρη τιμή fp_rate σε οποιοδήποτε αρχείο .wav δοθεί και ζητηθεί από τον χρήστη
{
    //Η συνάρτηση rate λειτουργά με παρόμιο τρόπο όπως την info όσον αφορά το κομμάτι του έλεγχου της εγκυρώτητας του αρχείου
    //Για αποφυγή δεύσμευσης μνήνης διαβάζω 2 με 4 bytes την φορά και τα επιστρέφω απευθείας με putchar() στο νεο αρχείο .wav που δημιουργώ
    //Εξαιρούνται μερικά δεδομένα τα οποία μεταβάλονται με βάση το fp_rate που μου δίνετε από τον χρήστη
    unsigned char buff[4];

    RIFF(buff);
    for (int k=0;k<4;k++)
    {
        putchar(buff[k]); //Κάθε φορά που τα 4 bytes σε μορφή μη προσημασμένων χαρακτήρων για το RIFF, WAVE, fmt και data 
    }                     //περάσαν επιτυχώς από τον έλεγχο, επιστρέφονται στο νέο αρχείο .wav που δημιουργήτε με putchar() 

    unsigned int size=Size_Of_File(); 
    int n=0; //Βοηθητική μεταβλητή η οποία αντιπροσωπεύει τα bits που θα μετατοπιστούν 
    for (int k=0;k<4;k++) 
    {
        putchar((size>>n)& 0xFF); //Μετατρέπω τα bytes από little-endian μορφή στην μορφή τα οποία τα διάβασα απομονόνοντας τα για να τα επιστρέψω στο νέο αρχείο με putchar()
        n=n+8; //Αυξάνετε κατά 8 αφού 1byte=8bits και έτσι κάθε φορά προχοράμε στο επόμενο byte
    }
    // Ο βρόχος χωρίζει τον 32-bit ακέραιο σε 4 bytes για να καταχώριστούν στο νεο αρχείο .wav
    //Πάρακάτω όλοι οι υπόλοιποι βρόχοι λειτουργούν με τον ίδιο τρόπο που λειτουργά είτε ο πρώτος είτε ο δεύτερος βρόχος που περιγράφτηκαν πιο πάνω
    WAVE(buff);
    for (int k=0;k<4;k++)
        {
            putchar(buff[k]);
        }

    fmt(buff);
    for (int k=0;k<4;k++)
    {
        putchar(buff[k]);
    }

    unsigned int chunk=format_chunk();
    n=0; 
    for (int k=0;k<4;k++) 
    {
        putchar((chunk>>n)& 0xFF);
        n=n+8; 
    }

    unsigned int WAVEt=WAVE_type();
    n=0; 
    for (int k=0;k<2;k++) 
    {
        putchar((WAVEt>>n)& 0xFF);
        n=n+8; 
    }

    unsigned int monost=mono_stereo();
    n=0; 
    for (int k=0;k<2;k++) 
    {
        putchar((monost>>n)& 0xFF);
        n=n+8; 
    }

    unsigned int sampleR=SampleRate();
    
    double temp= sampleR * fp_rate; //Υπολογίζω το νέο SampleRate με βάση το fp_rate που δώθηκε και αποθηκεύετε σε μια μεταβλητή temp
    if (temp<UINT_MAX) //Ελέγχω με βάση το fp_rate, εαν το νέο SampleRate ξεπερνά το όριο συγκρίνοντας το με το UINT_MAX (Μέγιστη τιμή που μπορεί να πάρει ένας unsigned int)
        {  
        }
        else
            {
                fprintf(stderr, "Error! new sample rate overflow\n");
                exit(1);
            }
    
    unsigned int new_sampleR= (unsigned int)round(temp); //Εαν περάσει επιτυχώς τον έλεγχο αποθηκεύω το temp στο new_sampleR σε μορφή μη προσημασμένου ακεραίου
            if (fp_rate==1.0) //στην περίπτωση που το αρχείο θα παραμείνει το ίδιο δηλαδή fp_rate=1.0, αποθηκεύω το παλιό Sample Rate στο νέο για ασφάλεια 
            {
                new_sampleR=sampleR;
            }
    n=0; 
    for (int k=0;k<4;k++) 
    {
        putchar((new_sampleR>>n)& 0xFF);
        n=n+8; 
    }

    unsigned int bytesSec=BytesPerSec(); 

    unsigned int blockA=BlockAlign(sampleR, bytesSec);

    unsigned int new_bytesSec= new_sampleR*blockA; //Υπολογίζω το νέο BytesPerSec με τον τύπο που μου έχει δωθεί επειδή είναι ο ασφαλέστερος τρόπος    
    
    n=0; 
    for (int k=0;k<4;k++) 
    {
        putchar((new_bytesSec>>n)& 0xFF);
        n=n+8; 
    } 
    n=0; 
    for (int k=0;k<2;k++) 
    {
        putchar((blockA>>n)& 0xFF);
        n=n+8; 
    } 

    unsigned int BPS=BitsPerSample(monost, blockA);
    n=0; 
    for (int k=0;k<2;k++) 
    {
        putchar((BPS>>n)& 0xFF);
        n=n+8; 
    } 

    DataChunk(buff);
    for (int k=0;k<4;k++)
    {
        putchar(buff[k]);
    }

    unsigned int Dsize=SizeOfData();
    n=0; 
    for (int k=0;k<4;k++) 
    {
        putchar((Dsize>>n)& 0xFF);
        n=n+8; 
    } 
    int a; //Βοηθητική μεταβλητή για το διάβασμα των byte εως το Dsize, με την χρήση του readB1 
    for (unsigned int i=0;i<Dsize;i++) //Μετά το header, διαβάζω ότι data περιμένω δηλαδή εως το Dsize και έπειτα το επιστρέφω στο νέο αρχείο απύρακτο
    {
        a=readB1();
        if (a<0) //Εαν μου επιστραφεί a=-1 σημαίνει το αρχείο τελείωσε πριν το προβλεπόμενο αριθμό επαναλήψεων του readB1 (Dsize) πράγμα που κάνει το αρχείο μη έγκυρο
        {
            fprintf(stderr, "Error! insufficient data\n");
            exit(1);
        }
        putchar(a); 
    }
    int b; //Βοηθητική μεταβλητή για το δίαβασμα των extra bytes που βρίσκονται μετά το Dsize (εαν υπάρχουν) 
    while ((b=readB1()) != -1) //Επανάληψη εως το τέλος του αρχείου σε περίπτωση που έχω bytes μετά το Dsize
    {
        putchar(b); //Εαν υπάρχουν extra bytes σε ένα αρχείο τα επιστρέφω απευθείας στο νέο αρχείο
    }
    //Χρησιμοποιώ την πιο πάνω επανάληψη επειδή εαν έχω bytes που δεν προβλέπω, δηλαδή βρίσκονται μετά το Dsize, μόνο έτσι τα διαβάζω και τα μεταφέρω στο νέο αρχείο
    CompareSizes(size);

    return 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int channel(int ans) //Συνάρτηση στην οποία υλοποιήτε η υποεντολή channel με ότι κανάλι ήχου (left or right) ζητηθεί, σε οποιοδήποτε αρχείο .wav δοθεί από τον χρήστη
{
    //Για την υλοποίηση αυτής της υποεντολής θα πρέπει υποχρεωτικά να αποθηκεύσω όλα τα byte του header σε ένα buffer 44 θέσεων (όσο είναι και το header)
    //Το κάνω με αυτό τον τρόπο επειδή μερικά δεδομένα του header μπορεί να αλλάξουν. Εξαρτάτε από το monostereo που βρίσκετε στην μέση του header
    //Άρα δεδομένα που βρίσκοντε πριν το monostereo ενδεχομένος να αλλάξουν για αυτό είναι απαραίτητο το buffer
    unsigned char header_buff[44]; 
    //Όσον αφορά το δίαβασμα και τον έλεγχο των δεδομένων, γίνοντε με παρόμιο τρόπο όπως στην info και rate
    unsigned char buff[4];
    //Πάρακάτω, αφού όλα τα δεδομένα διαβάζονται και περνούν τον έλεγχο θα αποθηκεύονται ένα προς ένα στο header_buffer, στην θέση από την οποία διαβάστηκαν 
    RIFF(buff);
    for (int k=0;k<4;k++)
    {
        header_buff[k]=buff[k]; 
    }

    unsigned int size=Size_Of_File(); 
    int n=0;
    for (int k=4;k<8;k++) 
        {
            header_buff[k]=(size>>n)& 0xFF;
            n=n+8; 
        }

    WAVE(buff);
    for (int k=8;k<12;k++)
        {
            header_buff[k]=buff[k-8];
        }

    fmt(buff);
    for (int k=12;k<16;k++)
        {
            header_buff[k]=buff[k-12];
        }
        
    unsigned int chunk=format_chunk();
    n=0;
    for (int k=16;k<20;k++) 
        {
            header_buff[k]=(chunk>>n)& 0xFF;
            n=n+8; 
        }

    unsigned int WAVEt=WAVE_type();
    n=0;
    for (int k=20;k<22;k++) 
        {
            header_buff[k]=(WAVEt>>n)& 0xFF;
            n=n+8; 
        }

    unsigned int monost=mono_stereo();
    n=0;
    for (int k=22;k<24;k++) 
        {
            header_buff[k]=(monost>>n)& 0xFF;
            n=n+8; 
        }

    unsigned int sampleR=SampleRate();
    n=0;
    for (int k=24;k<28;k++) 
        {
            header_buff[k]=(sampleR>>n)& 0xFF;
            n=n+8; 
        }

    unsigned int bytesSec=BytesPerSec();
    n=0;
    for (int k=28;k<32;k++) 
        {
            header_buff[k]=(bytesSec>>n)& 0xFF;
            n=n+8; 
        }

    unsigned int blockA=BlockAlign(sampleR, bytesSec);
    n=0;
    for (int k=32;k<34;k++) 
        {
            header_buff[k]=(blockA>>n)& 0xFF;
            n=n+8; 
        }

    unsigned int BPS=BitsPerSample(monost, blockA);
    n=0;
    for (int k=34;k<36;k++)
        {
            header_buff[k]=(BPS>>n)& 0xFF;
            n=n+8; 
        }

    DataChunk(buff);
    for (int k=36;k<40;k++)
        {
            header_buff[k]=buff[k-36];
        }

    unsigned int Dsize=SizeOfData();
    n=0;
    for (int k=40;k<44;k++) 
        {
            header_buff[k]=(Dsize>>n)& 0xFF;
            n=n+8; 
        }
    //Τώρα όλα τα δεδομένα του header του αρχείου είναι αποθηκευμένα στο header_buffer στην θέση από την οποία διαβαστήκαν
    if (monost==1) //εαν το κανάλι μου είναι μονοφωνικό τότε επιστρέφω το αρχείο όπως το δίαβασα χωρίς καμία αλλαγή στα bytes
        {
            for (int k=0;k<44;k++) //Με αυτή την επανάληψη επιστρέφω και τα 44 bytes του header στο νέο αρχείο χωρίς καμία αλλαγή
                {
                    putchar(header_buff[k]);
                }
            int a; //Βοηθητική μεταβλητή με την οποία διαβάζω όλα τα bytes μετά το header, εως το τέλος του αρχείου
            while ((a=readB1())!=-1) //Επανάληψη για να διαβάζω και να τυπώνω όλα τα bytes που βρίσκοντε μετά το header, εως το τέλος του αρχείου
                {
                    putchar(a);
                }
        }
        else //εαν το κανάλι είναι δυφωνικό, τότε... 
            {
                unsigned int new_size=(Dsize/2)+36; 
                //Το νέο size του αρχείου είναι Dsize/2(κρατάω τα μισά bytes του SampleData, είτε τα αριστερά είτε τα δεξιά)+36(header)(στο size δεν υπολογίζονται τα πρώτα 8 bytes του header)
                int n=0;
                for (int k=4;k<8;k++) 
                    {
                        header_buff[k]=(new_size>>n)& 0xFF; //Αλλάζω το size στο header_buff με το new_size που υπολόγισα
                        n=n+8; 
                    }

                unsigned int new_monost=1; //Επειδή κρατάμε μόνο το αριστερό είτε μόνο το δεξί κανάλη ήχου, ο νέος ήχος που δημιουργείτε είναι αυτόματα μονοφωνικός
                n=0;
                for (int k=22;k<24;k++) 
                    {
                        header_buff[k]=(new_monost>>n)& 0xFF;  //Αλλάζω το monost στο header_buff με το new_monost δηλαδή από 2 γίνετε 1
                        n=n+8; 
                    }

                unsigned int new_bytesSec=sampleR*(BPS/8); //Αλλάζει το BytesPerSecond επειδή αλλάζει και το BlockAlign (νέο BlockAlign= BPS/8)
                n=0;
                for (int k=28;k<32;k++) 
                    {
                        header_buff[k]=(new_bytesSec>>n)& 0xFF; //Αλλάζω το bytesSec στο header_buff με το new_bytesSec που υπολόγισα
                        n=n+8; 
                    }

                unsigned int new_blockA=BPS/8; //Βρίσκω το νέο BlockAlign με το να διαιρέσω το BPS με 8 αφού 1byte=8bits και από bitspersample γίνετε bytespersample
                n=0;
                for (int k=32;k<34;k++) 
                    {
                        header_buff[k]=(new_blockA>>n)& 0xFF; //Αλλάζω το blockA στο header_buff με το new_blockA που υπολόγισα
                        n=n+8; 
                    }

                unsigned int new_Dsize=Dsize/2; //Το νέο Dsize είναι ίσο με το μισό του παλαιού επειδή κρατάω τα μισά bytes ήχου, είτε μόνο τα αριστερά είτε μόνο τα δεξιά
                n=0;
                for (int k=40;k<44;k++) 
                    {
                        header_buff[k]=(new_Dsize>>n)& 0xFF; //Αλλάζω το Dsize στο header_buff με το new_Dsize που υπολόγισα
                        n=n+8; 
                    }

                for (int k=0;k<44;k++) 
                    {
                        putchar(header_buff[k]);
                    }
                //Αφότου έχω αλλάξει και αντικατέστησα όλες τις μεταβλητές που έπρεπε στο header_buff, το επιστρέφω όλο μαζί στο νέο αρχείο .wav που δημιουργήτε
                if (ans==1) //Εαν ο χρήστης επέλεξε να απομονώσει το αριστερό κανάλι ήχου... (ans: 1=left, 2=right)
                    {
                        if (BPS==8) //Και εαν το αρχείο ήχου είναι σε 8 BPS δηλαδή όλα τα άρτια bytes μετά το header είναι του αριστερά καναλιού ενώ όλα τα περιττά του δεξιού...
                            {       //Βytes after header: 0-left, 1-right, 2-left, 3-right...
                                //Άρα σε αυτή την περίπτωση, εαν τα bytes είναι άρτια τα διαβάζω και τα επιστρέφω αλιώς εαν είναι περιττά τα διαβάζω μόνο
                                int a; 
                                for (unsigned int k=0;k<Dsize;k++) //Επανάληψη μέχρι το original Dsize για να διαβάζω όλα τα bytes στο Sample Data του αρχείου
                                {
                                    a=readB1(); //Διαβάζω ένα ένα byte                                   
                                    if (a==-1) //Έλεγχος εαν τα αρχείο περιέχει λιγότερα bytes μετά το header απότι περιμένω
                                    {
                                        fprintf(stderr, "Error! insufficient data\n");
                                        exit(1);
                                    }
                                  if (k%2==0) //Εαν η θέση του byte είναι άρτια δηλαδή το byte ανήκει στο αριστερό κανάλι ήχου, τότε το επιστρέφει αλιώς δεν κάνει τίποτα
                                  {
                                    putchar(a);
                                  }
                                }
                            }
                        else if (BPS==16) //Εαν το αρχείο ήχου είναι σε 16 BPS, κάθε δύο byte το κανάλι ήχου αλλάζει
                            {             //Βytes after header: 0-left, 1-left, 2-right, 3-right, 4-left, 5-left, 6-right, 7-right...
                                //Για να απομονώσω τα bytes που χρειάζομαι σε αυτή την περίπτωση, χρησιμοποιώ μια μεταβλητή τύπου counter που μηδενίζετε με κάθε διάβασμα 4ων bytes
                                int a;
                                int i=0; //Η μεταβλητή τύπου counter
                                for (unsigned int k=0;k<Dsize;k++)
                                {
                                    a=readB1();                                   
                                    if (a==-1) 
                                    {
                                        fprintf(stderr, "Error! insufficient data\n");
                                        exit(1);
                                    }
                                    //Μέχρι εδώ λειτουργώ όπως την περίπτωση του BPS=8
                                    if (i<2) //Μέχρι το i είναι 0 ή 1 τότε τα bytes βρίσκονται στο αριστερό κανάλι ήχου για αυτό και τα επιστρέφω
                                    {
                                        putchar(a);                                       
                                    }
                                    //Εαν το i είναι >=2 σημαίνει ότι τα bytes βρίσκοντε στο δεξί κανάλι για αυτό και δεν τα επιστρέφω
                                    i++; //Μετά από κάθε διάβασμα byte και έλεγχο του i, αυξάνω το i κατά 1 που μου δείχνει ότι πήγα στο byte της επόμενης θέσης
                                    if (i==4) //Κάθε φορά που το i γίνετε 4 σημαίνει ότι διάβασα 4 bytes για αυτό και μηδενίζω το i για να επιστρέψω αργότερα τα επόμενα 2 bytes (αριστερό κανάλι)
                                    {
                                        i=0;
                                    }
                                }
                            }
                            while (readB1()!=-1); //Επανάληψη για το διάβασμα των extra bytes (εαν υπάρχουν) μετά το Dsize, για να γίνει ο έλεγχος Compare στο τέλος της συνάρτησης
                    }
                else if (ans==2) //Εαν ο χρήστης επέλεξε να απομονώσει το δεξί κανάλι ήχου... (ans: 1=left, 2=right)
                    {
                        //Ολόκληρη η περίπτωση ans=2 λειτουρεί σχεδόν με τον ίδιο τρόπο όπως την ans=1, μόνο που εδώ απομονώνω το δεξί κανάλι ήχου και όχι το αριστερό
                        if (BPS==8) 
                            {
                                int a;
                                for (unsigned int k=0;k<Dsize;k++)
                                {
                                    a=readB1();                                   
                                    if (a==-1)
                                    {
                                        fprintf(stderr, "Error! insufficient data\n");
                                        exit(1);
                                    }
                                  if (k%2==1) //Ελέγχω εαν η θέση του byte είναι περιττή αντί άρτια αφού σε αυτή την περίπτωση χρειάζομαι το δεξί κανάλι ήχου
                                  {
                                    putchar(a);
                                  }
                                }
                            }
                        else if (BPS==16) 
                            {
                                int a;
                                int i=0;
                                for (unsigned int k=0;k<Dsize;k++)
                                {
                                    a=readB1();                                   
                                    if (a==-1)
                                    {
                                        fprintf(stderr, "Error! insufficient data\n");
                                        exit(1);
                                    }
                                    if (i>=2) //Τώρα απομονώνω το 3ο και 4ο byte που ανήκουν στο δεξί κανάλι ήχου. Ξέρω ότι είμαι στο 3ο ή 4ο byte όταν i>=2
                                    {
                                        putchar(a);                                       
                                    }
                                    i++;
                                    if (i==4)
                                    {
                                        i=0;
                                    }
                                }
                            }
                            while (readB1()!=-1);
                    } 
                    CompareSizes(size); //Τελειώνω την περίπτωση που ο ήχος είναι στερεοφωνικός με έναν τελευτέο έλεγχο comparesizes σε περίπτωση που έχω αρζείο με extra bytes μετά το Dsize
            }
    //Τώρα εφόσον το αρχείο εισόδου είναι έγκυρο, μπορεί να υλοποιηθεί οποιαδήποτε περίπτωση της υποεντολής channel και να επιστραφεί ένα νεο αρχείο ήχου όπως το περιμένουμε
    return 0; 
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int main(int argc, char *argv[]) //argc: Μεταβλητή που μου υποδεικνύει πόσα διαφορετικά strings μου έδωσε ο χρήστης με το που έτρεξε το πρόγραμμα από το command line του terminal
{                                //argv: Δυσδυάστατος πίνακας χαρακτήρων που καταχωρεί το κάθε string που έδωσε ο χρήστης όταν έτρεξε το πρόγραμμα ξεχωριστά, χαρακτήρα προς χαρακτήρα 
    if (argc<2) //Η περίπτωση στην οποία ο χρήστης έδωσε ακριβός ένα string δηλαδή έγραψε και έτρεξε μόνο το ./soundwave πράγμα που δεν είναι έγκυρο για το συγκεκριμένο πρόγραμμα
        {
            fprintf(stderr, "Error! No subcommand provided\n");
            return 1;
        }

    readB1counter=0; //Κάθε φορά που τρέχει το πρόγραμμα δηλαδή με το τρέξιμο κάθε υποεντολής κάνω reset την μεταβλητή στο 0 επειδή μπορώ να τρέξω το ίδιο αρχείο ήχου με διάφορες υποεντολές

    if (strcmp(argv[1], "info")==0) //Με την εντολή strcmp συγκρίνω εαν το δεύτερο string που μου έχει δωθεί είναι το info 
        {                           //δηλαδή ελέγχω εαν ο χρήστης θέλει να τρέξει την υποεντολή info στο αρχείο ήχου που μου έδωσε
            return info(); //Εαν η συνθήκη είναι σωστή, τρέχει κανονικά η υποεντολή info στο αρχείο ήχου που μου δώθηκε και αν τρέξει επιτυχώς επιστρέφει 0
        }

    else if (strcmp(argv[1], "rate")==0) //Ελέγχω εαν το δεύτερο string που δώθηκε είναι το rate, δηλαδή ο χρήστης να θέλει να τρέξει την υποεντολή rate στο αρχείο ήχου που έδωσε
        {
            if (argc<3) //Εαν ο χρήστης θέλει να τρέξει την υποεντολή rate πρέπει υποχρεωτικά να δώσει και 3ο string, το fp_rate, αλιώς δεν θα τρέξει η rate
                {
                fprintf(stderr, "Error! No subcommand provided\n");
                return 1;
                }
            else
            {
                double fp_rate=atof(argv[2]); //Μετατρέπω το fp_rate που μου δώθηκε από πίνακα χαρακτήρων σε double με την βοήθεια της εντολής atof()
                if (fp_rate>0) //Ελέγχω για την ορθότητα του fp_rate (ένα fp_rate δεν γίνετε να πάρει αρνητικές τιμές ούτε να είναι 0)
                    {
                    return rate(fp_rate); //Εαν η συνθήκες είναι σωστές, τρέχει κανονικά η υποεντολή rate στο αρχείο ήχου που μου δώθηκε και αν τρέξει επιτυχώς επιστρέφει 0 
                    }                     //και δημιουργήτε ένα νέο αρχείο ήχου με βάση το fp_rate που ζήτησε ο χρήστης
                else 
                    {
                    fprintf(stderr, "Error! Rate input should be >0\n");
                    return 1;
                    }
            }
        }
    else if (strcmp(argv[1], "channel")==0) //Ελέγχω εαν το δεύτερο string που δώθηκε είναι το channel, δηλαδή ο χρήστης να θέλει να τρέξει την υποεντολή channel στο αρχείο ήχου που έδωσε
        {
            int ans; //Θα μεταφέρω το 3ο string στην channel αντιπροσωπεύοντας το με ένα ακέραιο για δική μου ευκολία
            if (argc<3) //Εαν ο χρήστης θέλει να τρέξει την υποεντολή channel πρέπει υποχρεωτικά να δώσει και 3ο string, το "left" ή "right", αλιώς δεν θα τρέξει η channel
                {
                    fprintf(stderr, "Error! No subcommand provided\n");
                    return 1; 
                }
            else if (strcmp(argv[2], "left")==0) //Ελέγχω εαν το 3o string είναι το "left", δηλαδή ο χρήστης θέλει να απομονώσει το αριστερό κανάλι ήχου του αρχείου
                {
                    ans=1; //Αντιπροσωπεύω το "left" string με τον ακέραιο 1 
                    return channel(ans); //Εαν η συνθήκες είναι σωστές, τρέχει κανονικά η υποεντολή channel στο αρχείο ήχου που μου δώθηκε και αν τρέξει επιτυχώς επιστρέφει 0
                }                        //και δημιουργήτε ένα νέο αρχείο ήχου μόνο με το αριστερό κανάλι ήχου του αρχείου
            else if (strcmp(argv[2], "right")==0) //Ελέγχω εαν το 3o string είναι το "right", δηλαδή ο χρήστης θέλει να απομονώσει το δεξί κανάλι ήχου του αρχείου
                {
                    ans=2; //Αντιπροσωπεύω το "right" string με τον ακέραιο 2
                    return channel(ans); //Εαν η συνθήκες είναι σωστές, τρέχει κανονικά η υποεντολή rate στο αρχείο ήχου που μου δώθηκε και αν τρέξει επιτυχώς επιστρέφει 0
                }                        //και δημιουργήτε ένα νέο αρχείο ήχου μόνο με το αριστερό κανάλι ήχου του αρχείου
            else //Για το 3ο string, οτιδήποτε δωθεί εκτός από "left" ή "right" θεωρείτε λάθος
                {
                    fprintf(stderr, "Error! Channel input should be \"left\" or \"right\"\n");
                    return 1;
                }
        }
    else //Για το 2ο string, οτιδήποτε δωθεί εκτός από "info", "rate" ή "channel", θεωρείτε λάθος επειδή δεν έχω άλλες υποεντολές στο πρόγραμμα
        {
            fprintf(stderr, "Error! Unknown subcommand: %s\n", argv[1]);
            return 1;
        }
    //Εαν για exitcode πάρω 0, το αρχείο ήταν έγκυρο και το πρόγραμμα έτρεξε επιτυχώς ενώ εαν πάρω 1 σημαίνει πως το αρχείο ήχου δεν είναι έγκυρο
    //Τέλος, το πρόγραμμα μπορεί να τρέξει με επιτυχία και ασφάλεια και τις 3εις υποεντολές "info", "rate" και "channel" σε ένα έγκυρο αρχείο ήχου .wav
    return 0;
}
