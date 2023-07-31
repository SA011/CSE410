#include <bits/stdc++.h>
#include "bitmap_image.hpp"
using namespace std;

using Tf = double;
using Ti = int;
using point = array<Tf, 4>;
using matrix = array<point, 4>;
const Tf PI = acos(-1);
const Tf EPS = 1e-9;
int dcmp(Tf x) {return abs(x)<EPS? 0 :(x<0?-1:1);}

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

struct Triangle{
	point p[3];
	array<int, 3> color;
};
static unsigned long int g_seed = 1;
inline int randm(){
	g_seed = (214013 * g_seed + 2531011);
	return (g_seed >> 16) & 0xFF;
}
struct Line{
	point a, b;
	Line(point _a, point _b){
		a = _a;
		a[2] = 0;
		b = _b;
		b[2] = 0;
	}
};
bool onLine(const point &p, const Line &l) {
	return dcmp(cross(l.a - p, l.b - p)[2]) == 0 &&
         dcmp(dot(l.a - p, l.b - p)) <= 0;
}

vector<point> lineIntersection(const Line &l1, const Line &l2){
	point v1 = l1.b - l1.a, v2 = l2.b - l2.a;
	vector<point> ret;
	if(dcmp(cross(v1, v2)[2]) == 0){
		//parallel
		if(onLine(l1.a, l2)){
			ret.push_back(l1.a);
		}
		if(onLine(l1.b, l2)){
			ret.push_back(l1.b);
		}
		if(onLine(l2.a, l1)){	
			ret.push_back(l2.a);
		}
		if(onLine(l2.b, l1)){
			ret.push_back(l2.b);
		}
		return ret;
	}

	point u = l1.a - l2.a; 
	ret.push_back(l1.a + (cross(v2, u)[2] / cross(v1,v2)[2]) * v1);
	if(onLine(ret[0], l1) && onLine(ret[0], l2))return ret;
	ret.clear();
	return ret;
}
vector<point> triangleIntersection(const Triangle &t, const Line &l){
	vector<point> ret;
	for(int i = 0; i < 3; i++){
		Line l2(t.p[i], t.p[(i + 1) % 3]);
		auto x = lineIntersection(l2, l);
		for(auto y : x)ret.push_back(y);
	}
	if(ret.empty())return ret;
	sort(ret.begin(), ret.end());
	vector<point> ans(1, ret[0]);
	for(int i = 1; ret.size(); i++){
		if(dcmp(ret[i][0] - ans.back()[0]) == 0)continue;
		ans.push_back(ret[i]);
	}
	assert(ans.size() <= 2);
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
	
	stage1.close();
	stage2.close();
	stage3.close();



	//part2
	vector<Triangle> triangles;
	Ti width, height;
	
	auto read = [&](){
		ifstream fconfig("config.txt");
		ifstream fstage3("stage3.txt");
		
		fconfig >> width >> height;
		Triangle t;
		while(fstage3 >> t.p[0]){
			fstage3 >> t.p[1] >> t.p[2];
			t.color[0] = randm();
			t.color[1] = randm();
			t.color[2] = randm();
			triangles.push_back(t);
		}

		// cerr << width << " " << height << endl << endl;

		// for(auto x : triangles){
		// 	cerr << x.p[0] << endl;
		// 	cerr << x.p[1] << endl;
		// 	cerr << x.p[2] << endl;
		// 	cerr << x.color[0] << " " << x.color[1] << " " << x.color[2] << endl;
		// }
	};

	read();

	//init

	Tf left_limit = -1;
	Tf right_limit = 1;
	Tf top_limit = 1;
	Tf bottom_limit = -1;
	Tf dx = (right_limit - left_limit) / width;
	Tf dy = (top_limit - bottom_limit) / height;
	Tf Top_Y = top_limit - dy / 2;
	Tf Bottom_Y = Top_Y - (height - 1) * dy;
	Tf Left_X = left_limit + dx / 2;
	Tf Right_X = Left_X + (width - 1) * dx;
	Tf z_max = 2.0;

	vector<vector<Tf>> z_buffer(height, vector<Tf>(width, z_max));


	bitmap_image image(width, height);
	image.set_all_channels(0, 0, 0);

	//init done




	for(auto &x : triangles){
		Tf maxY = Bottom_Y;
		Tf minY = Top_Y;
		for(int i = 0; i < 3; i++){
			maxY = max(maxY, x.p[i][1]);
			minY = min(minY, x.p[i][1]);
		}
		Ti top_scanline = round((Top_Y - maxY) / dy);
		Ti bottom_scanline = round((Top_Y - minY) / dy);
		// cerr << top_scanline << ' ' << bottom_scanline << endl;
		for(int i = top_scanline; i <= bottom_scanline; i++){
			vector<point> intersectionPoints = triangleIntersection(x, Line({Left_X, Top_Y - dy * i, 0, 0}, {Right_X, Top_Y - dy * i, 0, 0}));
			int leftCol = width, rightCol = -1;
			if(intersectionPoints.size() >= 1){
				leftCol = round((intersectionPoints[0][0] - Left_X) / dx);
				if(intersectionPoints.size() == 2){
					rightCol = round((intersectionPoints[1][0] - Left_X) / dx);
				}else{
					rightCol = leftCol;
				}
			}
			for(int j = leftCol; j <= rightCol; j++){
				//calculate Z values
			}
		}	
	}




	return 0;
}
