#include <iostream> 

using namespace std;


// 01 ±³°ü

 
int weight[1005];
int value[1005];
int dp[1005];

int n,m;

int main()
{
	
	cin>>n>>m;
	
	for(int i=1;i<=n;i++){
		cin>>value[i]>>weight[i];
	}
	
	
	for(int i=1;i<=n;i++){
		for(int j=m;j>=weight[i];j--){
			
			dp[j] = max(dp[j],dp[j-weight[i]]+value[i]);
			
		}
	}
	
	
	
	cout<<dp[m]<<endl;
	
	
	return 0;
}
