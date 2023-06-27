#include "Atoms.h"
#include "Parser.h"
#include <iostream>
#include "LexicalAnalyser.h"
#include "StringTable.h"

/*int x = 12;

int sum (int y) const {
	int i;
		return sum(x);
}

int main(){
	
	int z;
	z = sum(++x);

}
*/

/*int func(int a, int b, int c){
int result;
if(b * b - 4 * a * c > 0){
out "Lol\n";
result = -9 * b;}
return result;}

int main(){
int x, y, z, res;
in x;
in y;
in z;

res = func(x,y,z);

for(x = 0; x < 3; ++x)
{out x;
}
return 0;
}*/

void main() {
	const Scope GlobalScope = -1;

	/*int sqRoots(int x, int y, int z){
int result;
result = y*y - 4*x*z;
if (result < 0){
 out "No real roots\n";
} else {
 if (result == 0)
out "One root\n";
 else
out "Two roots\n";
}
return result;
}
int main(){
 int a, b, c, d;
 in a;
 in b;
 in c;
 d = sqRoots(a, b, c);
 return 0;
}*/

	/*int main(){int A, B, i;
in A;
in B;
for(i = B - 1; i > A; ++i){
out i;
}
out "\nVsego chisel = ";
out B - A - 1;
}*/

	/*SymbolTable st;
	st.add("a");
	st.add("aad");
	st.add("");
	std::cout << st;
	*/
	
	

	std::ifstream ifile("myprog.minic");
	Parser parser(ifile);
	parser.printResult(std::cout);
	

	/*std::ifstream ifile("myprog.minic");
	Scanner scanner(ifile);
	for (;;) {
		Token currentLexem = scanner.getNextToken();
		
		currentLexem.print(std::cout);
		if ((currentLexem.type() == LexemType::error) || (currentLexem.type() == LexemType::eof)) {
			break;
		}
	}
	*/

}


/*int b;

int func(int x){
return x * x;}

int a;
int main(){
 int c, d, i;
 in a;
 in b;
 in c;
 in d;
 i = func(i);

 switch (i++ + 3 * d){
  

 case 0: {

 while (!a == 0){
 a = a + 1;}
 }

 default: {
 for (i = 0; i < d; ++i){
 c = c * c;
 }
 }
 
 case 98: {
 if(b == 0){
 out "b is zero";}
 else{
 out b + 1;}}
 }
 
 }
 */