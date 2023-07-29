#include <bits/stdc++.h>
using namespace std;

using Tf = double;
using point = array<Tf, 4>;
using matrix = array<point, 4>;
const Tf PI = acos(-1);


matrix getIdentity(){
	matrix id;
	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++)
			id[i][j] = (i == j);
	return id;
}
matrix initMatrix(){
	matrix id;
	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++)
			id[i][j] = 0;
	return id;
}
point initPoint(){
	point id;
	for(int i = 0; i < 4; i++)id[i] = 0;
	return id;
}
Tf degToRad(Tf angle){
	return angle * PI / 180;
}
istream& operator >> (istream &s, point &a){
	s >> a[0] >> a[1] >> a[2]; 
	a[3] = 1;return s;
}


ostream& operator << (ostream &s, const point &a){
	s << a[0] << ' ' << a[1] << ' ' << a[2]; return s;
}

point operator * (Tf value, const point &p){
	point res;
	for(int i = 0; i < 4; i++)res[i] = p[i] * value;
	return res;
}
point operator + (const point &p, const point &q){
	point res;
	for(int i = 0; i < 4; i++)res[i] = p[i] + q[i];
	return res;
}
point operator - (const point &p, const point &q){
	point res;
	for(int i = 0; i < 4; i++)res[i] = p[i] - q[i];
	return res;
}

const Tf EPS = 1e-9;

point convPoint(Tf px, Tf py, Tf pz){
	return {px, py, pz, 1};
}

point convVector(Tf vx, Tf vy, Tf vz){
	return {vx, vy, vz, 0};
}

point cross(const point &a, const point &b){
	return {a[1] * b[2] - a[2] * b[1], - a[0] * b[2] + a[2] * b[0], a[0] * b[1] - a[1] * b[0], 0};
}
Tf dot(const point &a, const point &b){
	Tf ans = 0;
	for(int i = 0; i < 3; i++)ans += a[i] * b[i];
	return ans;
}


point norm(point p){
	if(p[3] == 0){
		p[3] = 1;
		return p;
	}
	for(int i = 0; i < 4; i++)p[i] /= p[3];
	return p;
}
point vectorNormalize(point p){
	double d = hypot(p[0], p[1], p[2]);
	if(abs(d) < EPS)return p;
	for(int i = 0; i < 3; i++)p[i] /= d;
	return p;
}
matrix norm(matrix p){
	if(p[3][3] == 0){
		p[3][3] = 1;
		return p;
	}
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++)p[i][j] /= p[3][3];
	}
	return p;
}
matrix mult(const matrix &a, const matrix &b){
	matrix ans = initMatrix();
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			for(int k = 0; k < 4; k++)ans[i][j] += a[i][k] * b[k][j];
		}
	}
	return norm(ans);
}
point mult(const matrix &a, const point &b){
	point ans = initPoint();
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++)ans[i] += a[i][j] * b[j];
	}
	return norm(ans);
}

matrix generateTranslator(point &p){
	matrix ans = getIdentity();
	for(int i = 0; i < 3; i++)ans[i][3] = p[i];
	return ans;
}
matrix generateScaleMatrix(point &p){
	matrix ans = getIdentity();
	for(int i = 0; i < 3; i++)ans[i][i] = p[i];
	return ans;
}
point Rodri(const point &x, const point &a, Tf angle){
	Tf cosAngle = cos(angle);
	Tf sinAngle = sin(angle);
	return cosAngle * x + (1 - cosAngle) * dot(a, x) * a + sinAngle * cross(a, x);
}
matrix generateRotateMatrix(Tf angle, point &p){
	angle = degToRad(angle);
	p = vectorNormalize(p);
	point c1 = Rodri({1, 0, 0, 0}, p, angle);
	point c2 = Rodri({0, 1, 0, 0}, p, angle);
	point c3 = Rodri({0, 0, 1, 0}, p, angle);
	matrix ans = getIdentity();
	for(int i = 0; i < 3; i++)ans[i][0] = c1[i];
	for(int i = 0; i < 3; i++)ans[i][1] = c2[i];
	for(int i = 0; i < 3; i++)ans[i][2] = c3[i];
	return ans;
}


int main(){
	ifstream fscene("scene.txt");
	ofstream stage1("stage1.txt");
	ofstream stage2("stage2.txt");
	ofstream stage3("stage3.txt");
	stage1 << fixed << setprecision(8);
	stage2 << fixed << setprecision(8);
	stage3 << fixed << setprecision(8);
	
	
	point eye, look, up;
	fscene >> eye >> look >> up;
	Tf fovY, aspectRatio, near, far;
	fscene >> fovY >> aspectRatio >> near >> far;


	
	//stage2 calculation
	matrix V;
	{
		point l = look - eye;
		l = vectorNormalize(l);
		point r = cross(l, up);
		r = vectorNormalize(r);
		point u = cross(r, l);
		
		matrix T = getIdentity();
		for(int i = 0; i < 3; i++)T[i][3] = -eye[i];
		
		matrix R = getIdentity();
		for(int i = 0; i < 3; i++)R[0][i] = r[i];
		for(int i = 0; i < 3; i++)R[1][i] = u[i];
		for(int i = 0; i < 3; i++)R[2][i] = -l[i];
		
		V = mult(R, T);
	}
	//
	
	//stage3 calculation
	matrix P;
	{
		Tf fovX = fovY * aspectRatio;
		Tf t = near * tan(degToRad(fovY / 2));
		Tf r = near * tan(degToRad(fovX / 2));
		P = getIdentity();
		P[0][0] = near / r;
		P[1][1] = near / t;
		P[2][2] = -(far + near) / (far - near);
		P[2][3] = -(2 * far * near) / (far - near);
		P[3][2] = -1;
		P[3][3] = 0;
	}
	
	//
	stack<matrix> stk;
	matrix M = getIdentity();
	
	while(true){
		string command;
		fscene >> command;
		if(command == "triangle"){
			point p;
			for(int i = 0; i < 3; i++){
				fscene >> p;
				
				stage1 << mult(M, p) << "\n";
				stage2 << mult(V, mult(M, p)) << "\n";
				stage3 << mult(P, mult(V, mult(M, p))) << "\n";
			}
			
			stage1 << "\n";
			stage2 << "\n";
			stage3 << "\n";
			
		}else if(command == "translate"){
			point p;
			fscene >> p;
			M = mult(M, generateTranslator(p));
		}else if(command == "scale"){
			point p;
			fscene >> p;
			M = mult(M, generateScaleMatrix(p));
		}else if(command == "rotate"){
			double angle;
			point p;
			fscene >> angle >> p;
			p[3] = 0;
			M = mult(M, generateRotateMatrix(angle, p));
		}else if(command == "push"){
			stk.push(M);
		}else if(command == "pop"){
			M = stk.top();
			stk.pop();
		}else if(command == "end"){
			break;
		}else{
			cerr << "INVALID COMMAND\n";
		}
	}
	
	return 0;
}
