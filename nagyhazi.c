#include "debugmalloc.h" //memória szivárgás ellenőrzés
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*strúktúra definíciók*/
typedef struct{
    char nev[51];
    double kcal;
    double tomeg;
    int ido;
    char datum[11];
    double csokolade_tartalom;
    char mentes_e[5];
    int nepszeruseg;
    int koltseg;
    int ar;
}suti;

typedef struct sutik_lista{
    suti adatok;
    struct sutik_lista *next;
}sutik_lista;

typedef struct{
    char nev[51];
    int nyitasi_ev;
    int tavolsag;
}bolt;

typedef struct cukraszda_lista{
    bolt cukraszda;
    struct cukraszda_lista *next;
    sutik_lista *head;
}cukraszda_lista;

/*függvény deklarációk*/
void cukraszdak_beolvasasa(cukraszda_lista *strazsa, char fajl_nev[], int max_tavolsag);
void sutik_beolvasasa (cukraszda_lista *strazsa, char fajl_nev[]);
sutik_lista* hasonlito(sutik_lista *uj, sutik_lista *korabbi);
void felszabadit_sutik(cukraszda_lista *strazsa);
void felszabadit_cukraszdak(cukraszda_lista *strazsa);

/*függvények*/
void cukraszdak_beolvasasa(cukraszda_lista *strazsa, char fajl_nev[], int max_tavolsag) //első tesztelésen átment :)
{
    FILE *f;
    bolt aktualis;
    cukraszda_lista *p;

    p=strazsa; //p mutató fogja követni a listát, hogy a strázsa ne változzon
    f=fopen(fajl_nev, "rt"); //megnyitja olvasásra a cukrászdákat tartalmazó fájlt
    if(f==NULL) // hiba ellenőrzés
    {
        perror("A cukraszdakat tartalmazo fajl megnyitasa sikertelen volt.\n");
        return;
    }
    else //sikerült a fájl nyitás
    {
        while(fscanf(f,"Cukraszda neve: %[^\n]\nNyitasi ev: %d\nFovarostol mert tavolsag km-re kerekitve: %d\n", aktualis.nev, &aktualis.nyitasi_ev, &aktualis.tavolsag)==3)
        //ezt a ciklust ismétli addig, amíg be tud olvasni a fájlból bolt struktúrába illeszthető adatokat, röviden a fájl végéig
        {
            if(aktualis.tavolsag<=max_tavolsag) //ellenőrzi, hogy a cukrászda elég közel van-e Budapesthez
            {
                p->next=malloc(sizeof(cukraszda_lista)); //memória foglalás a következő listába fűzendő elemnek
                p->next->cukraszda=aktualis; //lista elem adatainak átadása
                p->next->head=NULL; 
                p->next->next=NULL;
                p=p->next;
            }
        }
        fclose(f); //cukrászdákat tartalmazó fájl bezárása, hogy megnyitható legyen más fájl
    }
}
void sutik_beolvasasa (cukraszda_lista *strazsa, char fajl_nev[])
{
    FILE *f;
    cukraszda_lista *p;
    sutik_lista aktualis;
    sutik_lista *q;
    sutik_lista *masik_suti;

    f=fopen(fajl_nev, "rt"); //süteményeket tartalmazó fájl megnyitása
    if(f==NULL)//fájl nyitás sikerességének vizsgálata
    {
        perror("A sutemenyeket tartalmazo fajl megnyitasa sikertelen volt.\n");
        return;
    }
    else
    {
        while(fscanf(f,"Cukraszda neve: %[^\n]\nKaloriatartalom: %lf\nTomeg:%lf\nElkeszitesi ido: %d\nLejarati ido: %[^\n]\nCsokolade tartalom: %lf\nMentes-e? %s\nNepszerusegi pontszam: %d\nElkeszitesi koltseg: %d\nBolti ara: %d\n",aktualis.adatok.nev,&aktualis.adatok.kcal,&aktualis.adatok.tomeg,&aktualis.adatok.ido,aktualis.adatok.datum,&aktualis.adatok.csokolade_tartalom,aktualis.adatok.mentes_e,&aktualis.adatok.nepszeruseg,&aktualis.adatok.koltseg,&aktualis.adatok.ar)==10)
        {
            for(p=strazsa->next; p!=NULL; p=p->next) 
            /*bejárja a cukrászdákat tartalmazó listát a p pointer addig amíg nem találja meg az aktuális sütihez tartozó cukrászdát
            ha a lista végére ért a p, de a süteményhez tartozó cukrászda nem volt a listában akkor is vége a for ciklusnak*/
                if(strcmp(aktualis.adatok.nev, p->cukraszda.nev)==0)
                    if(p->head==NULL) //ez azt jelenti, hogy a cukrászdához még nem tartozik süti, vagyis jelenleg az aktuális süti tartalmazza a legkedvezőbben a legtöbb csokit ebben a cukrászdában
                    {
                        q=malloc(sizeof(sutik_lista)); //memória terület foglalása a sütemény adatainak és a lista fűzéshez szükséges next-nek
                        aktualis.next=NULL;
                        *q=aktualis; //adatok áthelyezése
                        p->head=q;                         
                    } //a cukrászdához lett láncolva az aktuális süti
                    else 
                    {
                        q=malloc(sizeof(sutik_lista)); //memória terület foglalása a sütemény adatainak és a lista fűzéshez szükséges next-nek
                        *q=aktualis; //adatok áthelyezése
                        p->head=hasonlito(q, p->head); //a függvény segítségével a legkedvezőbb csoki-ár tartalmú sütemény van a cukrászdához közvetlenül láncolva, tehát a sutik_lista elején
                    }
        } //elfogytak a még be nem olvasott sütik a süteményeket tartalmazó fájlból
        fclose(f); //fájl bezárása
    }

}
sutik_lista* hasonlito(sutik_lista *uj, sutik_lista *korabbi)
{
    if((uj->adatok.csokolade_tartalom/uj->adatok.ar)>(korabbi->adatok.csokolade_tartalom/korabbi->adatok.ar))
    {
        /* az új süti jobb arányú mint az eddigiek*/
        uj->next=korabbi;
        korabbi=uj;
    }
    else free(uj);
    /* ha rosszabb az új, ezért a feladat szempontjából lényegtelen így fel kell szabadítható a memóriában elfoglalt helyét, hiszen nem is láncoljuk a listához*/
    return korabbi;
}
void felszabadit_sutik(cukraszda_lista *strazsa)//sütik listák felszabadítása
{
    cukraszda_lista *p;
    sutik_lista *k, *n;
    for(p=strazsa->next; p!=NULL; p=p->next)//bejárja a cukrászdák listáját
    {
        for(k=p->head; k!=NULL; k=n)
        {
            n=k->next;
            free(k);
        }
    }
}

void felszabadit_cukraszdak(cukraszda_lista *strazsa) //cukrászdák listájának felszabadítása
{
    cukraszda_lista *p, *q;
    for(p=strazsa->next; p!=NULL; p=q)
    {
        q=p->next;
        free(p);
    }
}

int main()
{
    //szükséges változók
    cukraszda_lista strazsa, *eredmeny, *p,*q; //p és q listákat bejáró segédmutatók 
    strazsa.next=NULL;
    strazsa.head=NULL;
    char cukraszdak_nev[51]; //cukrászdákat tartalmazó fájl nevét tárolja
    char sutik_nev[51]; //süteményeket tartalmazó fájl nevét tárolja
    int max_tavolsag; //maximális távolság értékét tárolja
    double eddigi=0, uj;

    printf("Kerem adja meg a cukraszdakat tartalmazo fajl nevet:\n(ez legfeljebb 50 karakter hosszu lehet)\n");
    scanf("%s", cukraszdak_nev);
    printf("Kerem adja meg a sutemenyeket tartalmazo fajl nevet:\n(ez legfeljebb 50 karakter hosszu lehet)\n");
    scanf("%s", sutik_nev);
    printf("Kerem adja meg a maximalis tavolsag erteket egesz kilometerre kerekitve:\n");
    scanf("%d",&max_tavolsag);

    cukraszdak_beolvasasa(&strazsa, cukraszdak_nev, max_tavolsag);
    sutik_beolvasasa(&strazsa, sutik_nev);

    if(strazsa.next==NULL) //nincs a keresett távolságon belül cukrászda vagy a fájl megnyitássikertelen
    {
        printf("Nem talalhato az adott tavolsagon belul cukraszda vagy mar korabban ki volt irva, hogy a fajl nyitassal problema volt.\n");
        /*nem készült el egy láncolt lista amiben a cukrászdák egymás után lennének láncolva, 
        így semmit nem kell felszabadítani*/
        return 0;
    }

    //a legjob cukrászda megkeresés
    for(eredmeny=strazsa.next, p=strazsa.next->next; p!=NULL; p=p->next)
    {
        if(p->head==NULL) //ekkor a cukrászdához nem tartozik süti kínálat
            if(p->next!=NULL) continue; //még lehet, hogy van más jó cukrászda
            else break;

        if(eredmeny->head!=NULL)
            eddigi=eredmeny->head->adatok.csokolade_tartalom/eredmeny->head->adatok.ar;

        uj=p->head->adatok.csokolade_tartalom/p->head->adatok.ar;
        if(eddigi<uj)
        {    
            eredmeny=p;
        }
    }//a kérdésre a választ az eredmeny pointer adja és a cukrászdákhoz tartozó sütik listái már fel vannak szabadítva
    if(eredmeny->head==NULL) //A cukrászdában nincs süti
        printf("Nincs a keresesnek megfelelo talalat, mivel a tavolsagnak megfelelo cukraszda nem forgalmaz sutemenyt vagy mar korabban ki volt irva, hogy a sutemenyeket tartalmazo fajl megnyitasaval gondok voltak.\n");
    else
        printf("A keresett cukraszda neve: %s\n", eredmeny->cukraszda.nev);
    
    felszabadit_sutik(&strazsa);    
    felszabadit_cukraszdak(&strazsa);
    
    return 0;
}