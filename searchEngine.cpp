#include<iostream>
#include<stdlib.h>
#include<fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
#include <stdio.h>
#include "html.h"
#define HASH_SIZE 501
#define URL_LENGTH 1000
#define SEED_URL "htttp://chitkarauniversity.edu.in"
#define MAX_URL_PER_PAGE 20
#define INTERVAL_TIME 10
using namespace std;
//initallize the global variable named as DEPTH set as 3
int DEPTH=0;

//linked list structure
struct node
{
string url;
int depth;
int key;
node *next,*prev;
}*head=NULL;
//array make 
node *hashArray[HASH_SIZE]={0}; 
//find the depth
int findDepth(char *a)
{
int x=a[0]-'0';
//cout<<x<<endl;
return x;
}
//make a linked list
node *linked_list(char *no,char *url,int key)
{
string s(url);
int depth=findDepth(no);
node *temp=head,*r;
if(temp==NULL)
{
temp=new node;
temp->url=s;
temp->next=NULL;
temp->prev=NULL;
temp->depth=depth;
temp->key=key;
//cout<<temp->url<<"    "<<temp->depth<<endl;
head=temp;
return temp;
}
else
{

while(temp->next!=NULL)
temp=temp->next;
r=new node;
r->url=s;
r->next=NULL;
r->prev=temp;
r->depth=depth;
r->key=key;
temp->next=r;
//cout<<temp->url<<"     "<<temp->depth<<endl;
return r;
}
}



//check no. of arrguments
int checkargc(int argc)
{
if(argc==4)
return 1;
else
return 0;
}

//check depth
int checkdepth(char *ptr)
{
int i=0,c=0;
while(ptr[i]!='\0')
{
if(ptr[i]>=48 && ptr[i]<=57)
c++;
i++;
}

if(c!=1)
return 0;
else
return 1;
}

//CHECK VALID URL
bool check_url(char *a)
{
string s(a);
string s1="wget ";
s1=s1+s;
cout<<s1<<endl;
if(!system(s1.c_str()))
return 1;
else
return 0;
}


//check directory
void testDir(char *dir)
{
    struct stat statbuf;
    if ( stat(dir, &statbuf) == -1 )
    {
        fprintf(stderr, "-----------------n");
        fprintf(stderr, "Invalid directoryn");
        fprintf(stderr, "-----------------n");
        exit(1);
    }
    
    //Both check if there's a directory and if it's writable
    if ( !S_ISDIR(statbuf.st_mode) )
    {
        fprintf(stderr, "-----------------------------------------------------n");
        fprintf(stderr, "Invalid directory entry. Your input isn't a directoryn");
        fprintf(stderr, "-----------------------------------------------------n");
        exit(1);
    }
    
    if ( (statbuf.st_mode & S_IWUSR) != S_IWUSR )
    {
        fprintf(stderr, "------------------------------------------n");
        fprintf(stderr, "Invalid directory entry. It isn't writablen");
        fprintf(stderr, "------------------------------------------n");
        exit(1);
    }
}


//get the page
/*void getpage(char *url)
{
char urlbuffer[URL_LENGTH+300]={0}; 
strcat(urlbuffer, "wget -e use_proxy=off -e"); 
//http_proxy=10.0.0.220:3128
strcat(urlbuffer,"-O ~/Home/Documents/temp.txt");
strcat(urlbuffer, url);
//strcat(urlbuffer, " --proxy-user=user1 --proxy-password=user1");
system(urlbuffer);
}
 */

void getpage(char *url)
{
char urlbuffer[URL_LENGTH+300]={0};
strcat(urlbuffer,"wget ");
strcat(urlbuffer,url);
strcat(urlbuffer," -O /home/administrator/saved.txt ");
system(urlbuffer);
}

//hash table
int returnHash(char *url)
{
string s(url);
int seed=131;
unsigned long hash=0;
for(int i=0;i<s.length();i++)
{
hash=(hash*seed)+s[i];
}
return hash % HASH_SIZE; 
}

//load into linkedlist
void load()
{
char *a=new char[100] ;
char *b=new char[100];
string data;
int flag=0,l,key;
ifstream infile;
infile.open("url.txt");
while(getline(infile,data))
{
if(flag==2)
{
linked_list(a,b,1);
flag=0;
}

if(flag==0)
{
strcpy(b,data.c_str());
cout<<b<<endl;
++flag;
}
else
if(flag==1)
{
strcpy(a,data.c_str());
cout<<a<<endl;
++flag;
}
}
infile.close();
}

//print the linked list
void print()
{
node *temp=head;
while(temp!=NULL)
{
cout<<temp->url<<"  "<<temp->depth<<" "<<temp->key<<endl;
temp=temp->next;
}
}
//copy the data from file into character array DM
void copy_data(char *c,string s,long long int size)
{
ifstream fin(s.c_str(),ios::in);
while(!fin.eof())
{
fin.getline(c,size);
}
fin.close();

//for(long long int i=0;i<size;i++)
//cout<<c[i];
cout<<endl<<"Sucessfull"<<endl;
}


/*

*GetNextURL*
------------

Description: Given a HTML string buffer, the URL of the HTML,
and a position index, find the closest URL after the position
and copy the URL into the result buffer, which is also an input
argument. This function is the main component of the HTML parser.
This function is designed such that it is meant to be repeatedly 
called toextract URLs from the HTML one at a time, starting from the 
beginning of the HTML and terminating when the end of the HTML
is reached. The return value of this function is meant for the
repeated calls of this function; the real return value is the
third input argument, which is the result buffer, in which a
new URL will be written if one is found. This function can 
handle normal absolute and relative URLs generally  found in
the <a href=""> tags; however, more extreme cases, like this
<a href="../../../a.txt">, are not currently being extracted.

Input: html_buffer, urlofthispage, result_buffer, current_position

Return: Position of the URL found

** Pseudo Code **

(1) IF (first call) THEN
      Remove white space characters from the page
    END
(2) Find the <a> or <A> html tags by scanning through the html text
(3) Keep going until we may have found a URL
(4) IF (It actually is NOT a URL, which has multiple possibilities) THEN
      Recursively call self from the next position
    END
(5) IF (It is an absolute URL) THEN
      Set result buffer to contain this URL
      Return the current position
    ELSE (It is an relative URL) THEN
      Produce the result URL by combining the relative URL with the urlofthispage
      Set result buffer to contain this URL
      Return the current position
    END
(7) Return -1 to signal completion

*****

*/

int GetNextURL(char* html, char* urlofthispage, char* result,int pos) 
{
  char c;
  int len, i, j;
  char* p1;  //!< pointer pointed to the start of a new-founded URL.
  char* p2;  //!< pointer pointed to the end of a new-founded URL.

  // NEW
  // Clean up \n chars
  if(pos == 0) {
    removeWhiteSpace(html);
  }
  // /NEW

  // Find the <a> <A> HTML tag.
  while (0 != (c = html[pos])) 
  {
    if ((c=='<') &&
        ((html[pos+1] == 'a') || (html[pos+1] == 'A'))) {
      break;
    }
    pos++;
  }
  //! Find the URL it the HTML tag. They usually look like <a href="www.abc.com">
  //! We try to find the quote mark in order to find the URL inside the quote mark.
  if (c) 
  {  
    // check for equals first... some HTML tags don't have quotes...or use single quotes instead
    p1 = strchr(&(html[pos+1]), '=');
    
    if ((!p1) || (*(p1-1) == 'e') || ((p1 - html - pos) > 10)) 
    {
      // keep going...
      return GetNextURL(html,urlofthispage,result,pos+1);
    }
    if (*(p1+1) == '\"' || *(p1+1) == '\'')
      p1++;

    p1++;    

    p2 = strpbrk(p1, "\'\">");
    if (!p2) 
    {
      // keep going...
      return GetNextURL(html,urlofthispage,result,pos+1);
    }
    if (*p1 == '#') 
    { // Why bother returning anything here....recursively keep going...

      return GetNextURL(html,urlofthispage,result,pos+1);
    }
    if (!strncmp(p1, "mailto:",7))
      return GetNextURL(html, urlofthispage, result, pos+1);
    if (!strncmp(p1, "http", 4) || !strncmp(p1, "HTTP", 4)) 
    {
      //! Nice! The URL we found is in absolute path.
      strncpy(result, p1, (p2-p1));
      return  (int)(p2 - html + 1);
    } else {
      //! We find a URL. HTML is a terrible standard. So there are many ways to present a URL.
      if (p1[0] == '.') {
        //! Some URLs are like <a href="../../../a.txt"> I cannot handle this. 
	// again...probably good to recursively keep going..
	// NEW
        
        return GetNextURL(html,urlofthispage,result,pos+1);
	// /NEW
      }
      if (p1[0] == '/') {
        //! this means the URL is the absolute path
        for (i = 7; i < strlen(urlofthispage); i++)
          if (urlofthispage[i] == '/')
            break;
        strcpy(result, urlofthispage);
        result[i] = 0;
        strncat(result, p1, (p2 - p1));
        return (int)(p2 - html + 1);        
      } else {
        //! the URL is a absolute path.
        len = strlen(urlofthispage);
for (i = (len - 1); i >= 0; i--)
          if (urlofthispage[i] == '/')
            break;
        for (j = (len - 1); j >= 0; j--)
          if (urlofthispage[j] == '.')
              break;
        if (i == (len -1)) {
          //! urlofthis page is like http://www.abc.com/
            strcpy(result, urlofthispage);
            result[i + 1] = 0;
            strncat(result, p1, p2 - p1);
            return (int)(p2 - html + 1);
        }
        if ((i <= 6)||(i > j)) {
          //! urlofthis page is like http://www.abc.com/~xyz
          //! or http://www.abc.com
          strcpy(result, urlofthispage);
          result[len] = '/';
          strncat(result, p1, p2 - p1);
          return (int)(p2 - html + 1);
        }
        strcpy(result, urlofthispage);
        result[i + 1] = 0;
        strncat(result, p1, p2 - p1);
        return (int)(p2 - html + 1);
      }
    }
  }    
  return -1;
}

/*

*NormalizeWord*
---------------

Description: Make sure all the Roman letters in the URL are
of lower cases, for ease of carrying out string comparison in
the future when trying to decide if two URL are the same or not.
Basically a linear scan, starting from the beginning of the URL,
is performed. Whenever a capital letter character is encountered
(by checking its ASCII code value), it is replaced by the
corresponding lower case letter.

Input: input_url

** Pseudo Code **

(1) FOR (every character in the input string) DO
      IF (this character is a capital letter) DO
        Change this letter to lower case
      END
    DONE

*****

*/

void NormalizeWord(char* word) {
  int i = 0;
  while (word[i]) {
      // NEW
    if (word[i] < 91 && word[i] > 64) // Bounded below so this funct. can run on all urls
      // /NEW
      word[i] += 32;
    i++;
  }
}

/*

*NormalizeURL*
--------------

Description: Normalize the input URL string and return its validity.
The normalization contains two major components: first, if the
URL ends with a trailing slash '/' character, this trailing slash
will be removed from the URL; and second, if the URL points to
a file (with an extension), then only certain file extensions are
accepted; currently, acceptable normal file extensions start with
.htm, .HTM, .php, or .jsp. A URL is valid if it is long enough and,
if pointing to a file, points to a file of acceptable format;
otherwise the URL is considered invalid and therefore will not
be added to the url_list for future processing.

Input: input_url

Output: validity of the input URL: 0 - invalid
                                   1 - valid

** Pseudo Code **

(1) Return error signal if input url is too short.
(2) IF (input url ends with '/') THEN
      Remove the '/'
    END
(3) Find the positions of the last occurrences of '/' and '.'
(4) IF (the '/' and '.' are positioned s.t. they indicate the url points to a file) THEN
      IF (the file extension starts with .htm or .HTM or .php or .jsp) THEN
        Do nothing...
      ELSE
        Return bad url signal
      END
    END
(5) Return good url signal

*****

*/

int NormalizeURL(char* URL) 
{
  int len = strlen(URL);
  if (len <= 1 )
    return 0;
  //! Normalize all URLs.
  if (URL[len - 1] == '/') 
  {
    URL[len - 1] = 0;
    len--;
  }
  int i, j;
  len = strlen(URL);
  //! Safe check.
  if (len < 2)
    return 0;
  //! Locate the URL's suffix.
  for (i = len - 1; i >= 0; i--)
    if (URL[i] =='.')
      break;
  for (j = len - 1; j >= 0; j--)
    if (URL[j] =='/')
      break;
  //! We ignore other file types.
  //! So if a URL link is to a file that are not in the file type of the following
  //! one of four, then we will discard this URL, and it will not be in the URL list.
  if ((j >= 7) && (i > j) && ((i + 2) < len)) 
  {
    if ((!strncmp((URL + i), ".htm", 4))
        ||(!strncmp((URL + i), ".HTM", 4))
        ||(!strncmp((URL + i), ".php", 4))
        ||(!strncmp((URL + i), ".jsp", 4))
        ) 
    {
      len = len; // do nothing.
    } 
    else 
    {
      return 0; // bad type
    }
  }
  return 1;
}

/*

*removeWhiteSpace*
------------------

Description: Removes the white space characters from the input
string buffer that contains the html content. This function
basically scans through the entire html buffer content character
by character, and abandons any white space character it encounters.
The ASCII code of the characters are used to determine whether
a character is a white space or not; Characters with ASCII code
values below 32 are considered white space characters, and are
thus removed.

Input: string_buffer

** Pseudo Code **

(1) Create a target buffer one character than the input buffer, and clear it
(2) FOR (Every character in the input buffer) DO
      IF (the current character is not a while space character) THEN
        Append it to the end of the target buffer
      END
    DONE
(3) Overwrite the input buffer with the target buffer
(4) Release targer buffer
    
*****

*/

void removeWhiteSpace(char* Html) 
{
  int i;
  char *buffer=(char*)malloc(strlen(Html)+1);
  char *p=(char*)malloc(sizeof(char)+1);
  memset(buffer,0,strlen(Html)+1);
  for (i=0;Html[i];i++) 
  {
    if(Html[i]>32)
    {
      sprintf(p,"%c",Html[i]);
      strcat(buffer,p);
    }
  }
  strcpy(Html,buffer);
  free(buffer);
  free(p);
}

//check url present in the file or not
bool check_url_present(char *argv)
{
string data;
int flag=0;
//check url present in file or not
//open file into read mode--->
ifstream infile;
infile.open("url.txt");
infile>>data;
while(getline(infile,data))
{
if(argv==data)
{cout<<"Already visit this url"<<endl; flag=1; break;
}
}
infile.close();

if(flag==0)
return 0;
else
return 1;
}

//save new url into file
void save_new_url(char *argv2,char *argv1)
{
//save url to file
//open file into write mode----->
ofstream outfile;
outfile.open("url.txt",ios::app);
outfile<<argv2<<endl;
outfile<<argv1<<endl;
outfile.close();
}



//save the url of url into some memory
void get_the_next_Url_store(int j,int flag,int pos,char* memory,char* Url,char* result,char Max_Url[20][1000])
{
int l;
while(flag--)
{
GetNextURL(memory,Url,result,pos);
l=strlen(result);
pos=pos+1;
for(int i=0;i<l;i++)
Max_Url[j][i]=result[i];
++j;
}

//print test
for(int i=0;i<MAX_URL_PER_PAGE;i++)
{
l=strlen(Max_Url[i]);
for(j=0;j<l;j++)
{
cout<<Max_Url[i][j];
}
cout<<endl;
}
}

//check the url are same or not
bool check_LinkedList(char *Max_Url)
{
/* //checking 
int l=strlen(Max_Url),i;
for(i=0;i<l;i++)
cout<<Max_Url[i];
cout<<endl;
*/
int key=returnHash(Max_Url);
if(hashArray[key]==0)
return 1;
else
return 0;
}


//checking and creating the linked list
void check_url_exist_LinkedList(char Max_Url[MAX_URL_PER_PAGE][1000])
{
string data;
ifstream infile;
string name;
node *temp;
int i,check,j;
int index;
char *c=new char[1];
for(i=0;i<MAX_URL_PER_PAGE;i++)
{
check=check_LinkedList(Max_Url[i]);
if(check==0)
cout<<"ALready Present"<<endl;//working pending if present the also check.
else
{
if(DEPTH==0)
c[0]='1';
else
if(DEPTH==1)
c[0]='2';

//check
//cout<<c<<"    "<<"Not Present"<<endl;


save_new_url(Max_Url[i],c);
//get page  or fetch  the url
getpage(Max_Url[i]);
temp=linked_list(c,Max_Url[i],0);
index=returnHash(Max_Url[i]);
hashArray[index]=temp;

//changes that may do in the url
int l=strlen(Max_Url[i]);
for(j=0;j<l;j++)
{
if(Max_Url[i][j]=='/')
Max_Url[i][j]=',';
//cout<<Max_Url[i][j];
}

string s=".txt";
name=name+Max_Url[i]+s;
//cout<<name<<endl;
//Progressing------------------------------->
ofstream outfile;
outfile.open(name.c_str());
for(j=0;j<l;j++)
{
if(Max_Url[i][j]==',')
Max_Url[i][j]='/';
}

outfile<<"Link:-"<<Max_Url[i]<<endl;
outfile<<"Depth:-"<<c<<endl;
//open file into read mode------->
infile.open("saved.txt");
while(getline(infile,data))
{
outfile<<data;
}
infile.close();
outfile.close();

}
}
DEPTH++;

print();

}




//main
int main(int argc,char *argv[])
{
char Max_Url[20][1000];
//load into linkedlist
load();
string data,name;
int flag=0,index,i,l,m,j;
if(checkargc(argc))
{
cout<<"arrguments are valid"<<endl;
if(checkdepth(argv[1]))
{
cout<<"depth checked"<<endl;
//test the directory
testDir(argv[3]);
cout<<"Directory tested"<<endl;
if(check_url(argv[2]))
{
cout<<"Url Checked"<<endl;
//check the url present in the file or not
flag=check_url_present(argv[2]);
//file opening condition
ifstream infile;

if(flag==0)
{
//save url into file
save_new_url(argv[2],argv[1]);
//file writing file condition
ofstream outfile;


//save its data into the file for something usage in future
//get the page
getpage(argv[2]);
//make a linked list
node *temp=linked_list(argv[1],argv[2],0);
//Generated hash table
index=returnHash(argv[2]);
//cout<<index<<endl;
hashArray[index]=temp;
/*cout<<hashArray[index];
node *temp1;
temp1=hashArray[index];
cout<<temp1->depth<<endl<<temp1->url;
*/

//22 dec
//New file Created
//save url to file
//open file into write mode----->
string s=".txt";
name=name+argv[2]+s;
//cout<<name<<endl;

outfile.open(name.c_str());
outfile<<"Link:-"<<argv[2]<<endl;
outfile<<"Depth:-"<<argv[1]<<endl;
//open file into read mode------->
infile.open("saved.txt");
while(getline(infile,data))
{
//infile>>data;
outfile<<data;
}
infile.close();
outfile.close();
}
//23 dec
//printing test linked list
//print();
//checking files which are non-visited
//and finding the size of the file to allocate the dyanamically memory
node *temp=head;
while(temp!=NULL)
{
if(temp->key==0 && temp->key<3)
{
//visited url
temp->key=1;

//getting the size of the file
FILE *pFile;
long long int size;
string s=".txt";
name=temp->url+s;
//cout<<name<<endl
//refigning problem check error occour------->
pFile=fopen (name.c_str(),"rb");
if(pFile==NULL) perror("Error! opening File.");
else
{
fseek(pFile,0,SEEK_END);
size=ftell(pFile);
fclose(pFile);
}
//cout<<endl<<size<<endl;
//memory allocate dynamically
char *memory=new char[size];
//Copy the data into dynamic memory
copy_data(memory,name,size);
//memory allocate to store result
char *result=new char[URL_LENGTH];
//Get next url form url
name=temp->url;
//new allocation of memory
char *Url=new char[URL_LENGTH];
//copy data form string to character
strcpy(Url,name.c_str());
flag=MAX_URL_PER_PAGE;
j=0;

//get the next url and store this into some memory space
get_the_next_Url_store(j,flag,0,memory,Url,result,Max_Url);
//check the url we get from the page will be exist in our linkedlist or not (Checking and creating the linked list)
check_url_exist_LinkedList(Max_Url);

//error no such file present

temp=temp->next;
free(memory);
free(result);
}
else
temp=temp->next;
}

}
else
{
cout<<"Not a valid URL"<<endl;
}
}
else
{
cout<<"Not valid depth"<<endl;
}
}
else
{
cout<<"No. of arrguments are not valid"<<endl;
}

}

