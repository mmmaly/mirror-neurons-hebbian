#include<iostream>
#include<fstream>
#include<cstdlib>
#include<ctime>
#include<cmath>

using namespace std;

double randuni()
{
	return double(rand())/RAND_MAX;
}

int random(int n)
{
	return int(randuni()*n);
}

double randn()
{
	const int p = 12;
	double s=0;
	for(int i=0;i<p;i++)
		s+=randuni();
	return s-p/2;
}

void randomize()
{
	srand(time(0));
}

double sigmoid(double x)
{
	return 1/(1+exp(-10*(x-0.5)));
}

int main()
{
	
	const int n = 5;
	const int numF5 = n;
	const int numPF = 1000*n;
	const int numSTS = n;

	double wF5_PF[numF5][numPF];
	double wSTS_PF[numSTS][numPF];
	double thPF[numPF];
	double alpha = 0.01;

	randomize();	
	
	for(int i=0;i<numF5;i++)
		for(int j=0;j<numPF;j++)
			wF5_PF[i][j] = randn();
	
	for(int i=0;i<numSTS;i++)
		for(int j=0;j<numPF;j++)
			wSTS_PF[i][j] = randn();
	
	for(int i=0;i<numPF;i++)
		thPF[i] = randn();


	double fatiquePF[numPF];
	for(int i=0;i<numPF;i++)
		fatiquePF[i] = 0;


	for(int ii=0;ii<1000;ii++)
	{
		double sumext = 0;
		double sumsum=0;
		double sumfatique = 0;
		double sumth = 0;
		double sumw = 0;
		double ew=0;

		for(int what=0;what<3;what++)
			for(int action=0;action<n;action++){

		double exF5[numF5];
		for(int i=0;i<numF5;i++)
		{
			double x = randn()/60; //small noise
			if(i/(numF5/n) == action && what == 1) // when doing this action
				x+=1;
			exF5[i] = sigmoid(x);
		}

		double exSTS[numSTS];
		for(int i=0;i<numSTS;i++)
		{
			double x = randn()/60; //small noise
			if(i/(numSTS/n) == action && what!=0)//when doing or observing this action
				x+=1;
			exSTS[i] = sigmoid(x);
		}
		
		for(int pf=0;pf<numPF;pf++)
		{
			double sum = 0;
			for(int i=0;i<numF5;i++)
				sum+=wF5_PF[i][pf]*exF5[i];
			for(int i=0;i<numSTS;i++)
				sum+=wSTS_PF[i][pf]*exSTS[i];
			sum-=thPF[pf];

			double excitation = sigmoid(sum);
			
			sumext+=excitation;
			sumsum+=sum;

			double sw=0;
			for(int f5=0;f5<numF5;f5++)
			{
				wF5_PF[f5][pf]=(wF5_PF[f5][pf]+exF5[f5]*excitation*alpha);
				
				sumw+=wF5_PF[f5][pf];
				sw+=wF5_PF[f5][pf];
			}			

			for(int sts=0;sts<numSTS;sts++)
			{
				wSTS_PF[sts][pf]=(wSTS_PF[sts][pf]+exSTS[sts]*excitation*alpha);
				
				sumw+=wSTS_PF[sts][pf];
				sw+=wSTS_PF[sts][pf];
			}

			sw/=(numF5+numSTS);
			sw/=0.2;


			for(int f5=0;f5<numF5;f5++)
				wF5_PF[f5][pf]/=sw;
			for(int sts=0;sts<numSTS;sts++)
				wSTS_PF[sts][pf]/=sw;

			fatiquePF[pf]*=0.95;
			fatiquePF[pf]+=excitation;

			thPF[pf]+=(fatiquePF[pf]-20.0/n*2/3)*alpha;

			sumfatique+=fatiquePF[pf];
			sumth += thPF[pf];
		}
		}
		cerr << "ii=" << ii << "\tE(ext) = " << sumext/numPF/3/n << "\tE(sum)=" << sumsum/numPF/3/n << "\tE(fatique)="<<sumfatique/numPF/3/n << "\tE(th)="<<sumth/numPF/3/n<<"\tE(w)="<<(ew=sumw/(numF5+numSTS)/numPF/3/n) <<  endl;
//		alpha*=0.9999;
	}


	double responseAction[n][numPF];

	for(int action=0;action<n;action++)
	{
		double exF5[numF5];
		for(int i=0;i<numF5;i++)
		{
			double x = randn()/60; //small noise
			if(i/(numF5/n) == action ) // when doing this action
				x+=1;
			exF5[i] = sigmoid(x);
		}

		double exSTS[numSTS];
		for(int i=0;i<numSTS;i++)
		{
			double x = randn()/60; //small noise
			exSTS[i] = sigmoid(x);
		}

		
		double sumex=0;
		for(int pf=0;pf<numPF;pf++)
		{
			double sum = 0;
			for(int i=0;i<numF5;i++)
				sum+=wF5_PF[i][pf]*exF5[i];
			for(int i=0;i<numSTS;i++)
				sum+=wSTS_PF[i][pf]*exSTS[i];
			sum-=thPF[pf];

			double excitation = sigmoid(sum);
			sumex+=excitation;

			responseAction[action][pf] = excitation;
		}
	}
	
	double responseObservation[n][numPF];

	for(int action=0;action<n;action++)
	{
		double exF5[numF5];
		for(int i=0;i<numF5;i++)
		{
			double x = randn()/60; //small noise
			exF5[i] = sigmoid(x);
		}

		double exSTS[numSTS];
		for(int i=0;i<numSTS;i++)
		{
			double x = randn()/60; //small noise
			if(i/(numSTS/n) == action)//when doing or observing this action
				x+=1;
			exSTS[i] = sigmoid(x);
		}

		
		double sumex=0;
		for(int pf=0;pf<numPF;pf++)
		{
			double sum = 0;
			for(int i=0;i<numF5;i++)
				sum+=wF5_PF[i][pf]*exF5[i];
			for(int i=0;i<numSTS;i++)
				sum+=wSTS_PF[i][pf]*exSTS[i];
			sum-=thPF[pf];

			double excitation = sigmoid(sum);
			sumex+=excitation;

			responseObservation[action][pf] = excitation;
		}
	}

	for(int pf=0;pf<numPF;pf++)
	{
		
		int count = 0;
		double maxAction = -1;
		double maxExcitation = 0;
		for(int action=0;action<n;action++)
		{
			if(responseAction[action][pf] > 0.5)
				count++;		
			if(responseAction[action][pf] > maxExcitation)
			{
				maxAction=action;
				maxExcitation=responseAction[action][pf];
			}
		}

		double maxObservation = -1;
		maxExcitation = 0;
		for(int action=0;action<n;action++)
		{
			if(responseObservation[action][pf] > 0.5)
				count++;		
			if(responseObservation[action][pf] > maxExcitation)
			{
				maxObservation=action;
				maxExcitation=responseObservation[action][pf];
			}
		}


		cout << "pf=" << pf << "\tcountActions=" << count <<"\tmaxAction=" << maxAction << "\tmaxObservation=" << maxObservation;
		if(maxAction==maxObservation && count==1)
			cout << "\tmirror neuron";
		cout << endl;
	}


}

