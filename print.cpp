#include<iostream>

using namespace std;


const string line1 = "┏━━━┯━━━┯━━━┳━━━┯━━━┯━━━┳━━━┯━━━┯━━━┓";

const string line2 = "┠───┼───┼───╂───┼───┼───╂───┼───┼───┨";

const string line3 = "┣━━━┿━━━┿━━━╋━━━┿━━━┿━━━╋━━━┿━━━┿━━━┫";

const string line4 = "┗━━━┷━━━┷━━━┻━━━┷━━━┷━━━┻━━━┷━━━┷━━━┛";


void number_line(){
	string bar1 = "┃ ";
	string bar2 = "│ ";
	int num[9] = {1,2,3,4,5,6,7,8,9};

	for(int i=0; i!=3; i++)
		for(int j =0; j!= 3; j++)
			cout<< ((j==0)?bar1:bar2) << num[i*3+j] << " ";

	

   
	cout<<bar1<<endl;

}
void  print_board(){

	for(int i = 0; i!=3; ++i)
		for(int j = 0; j!=6; ++j)
		{
			if(j%2==0)
				cout << ((j==0)?((i==0)?line1:line3):line2) << endl;
			else
				number_line();
		}

	cout << line4 << endl;

}

int main()
{
    print_board();
    return 0;
}
