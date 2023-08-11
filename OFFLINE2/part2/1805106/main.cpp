#include <bits/stdc++.h>
#include "bitmap_image.hpp"
using namespace std;

using Tf = double;
using Ti = int;
using point = array<Tf, 4>;
using matrix = array<point, 4>;
const Tf PI = acos(-1);
const Tf EPS = 1e-9;
const Tf INF = 1e9;
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
using Color = array<int, 3>;
struct Triangle{
	point p[3];
	Color color;
};
static unsigned long int g_seed = 1;
inline int randm(){
	g_seed = (214013 * g_seed + 2531011);
	return (g_seed >> 16) & 0xFF;
}

bool getIntersection(Triangle &t, Tf y, point &left, point &right){
	left = {INF, INF, INF, 1};
	right = {-INF, -INF, -INF, 1};
	for(int i = 0; i < 3; i++){
		int a = i, b = (i + 1) % 3;
		if(t.p[a][1] > t.p[b][1])swap(a, b);
		if(t.p[a][1] <= y && y < t.p[b][1]){
			Tf dy = t.p[b][1] - t.p[a][1];
			Tf dx = t.p[b][0] - t.p[a][0];
			Tf dz = t.p[b][2] - t.p[a][2];
			Tf tm = (y - t.p[a][1]) / dy;
			point p1 = t.p[a] + tm * (t.p[b] - t.p[a]);
			if(left[0] > p1[0])left = p1;
			if(right[0] < p1[0])right = p1;
		}
	}

	if(dcmp(left[0] - INF) == 0)return false;
	return true;
}

Tf getZ(point &left, point &right, Tf x){
	Tf dx = right[0] - left[0];
	Tf dz = right[2] - left[2];
	if(dcmp(dx) == 0)return left[2];
	Tf tm = (x - left[0]) / dx;
	return left[2] + tm * dz;
}

int main(){
	ifstream fscene("scene.txt");
	ofstream stage1("stage1.txt");
	ofstream stage2("stage2.txt");
	ofstream stage3("stage3.txt");
	ofstream z_buffer_file("z_buffer.txt");
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
	Tf z_max = 1.0;
	Tf z_front_limit = -1.0;

	vector<vector<Tf>> z_buffer(height, vector<Tf>(width, z_max));


	bitmap_image image(width, height);
	image.set_all_channels(0, 0, 0);

	//init done

	// cerr << height << " " << width << endl;

	for(auto &x : triangles){
		Tf maxY = Bottom_Y;
		Tf minY = Top_Y;
		for(int i = 0; i < 3; i++){
			maxY = max(maxY, x.p[i][1]);
			minY = min(minY, x.p[i][1]);
		}
		maxY = min(maxY, Top_Y);
		minY = max(minY, Bottom_Y);
		Ti top_scanline = round((Top_Y - maxY) / dy);
		Ti bottom_scanline = round((Top_Y - minY) / dy);
		// cerr << top_scanline << ' ' << bottom_scanline << endl;
		for(int i = top_scanline; i <= bottom_scanline; i++){
			point left, right;
			// cerr << "OK2" << endl;
			if(!getIntersection(x, Top_Y - i * dy, left, right))continue;
			// cerr << "OK3" << endl;
			assert(left[0] <= right[0]);
			int left_scanline = round((left[0] - Left_X) / dx);
			int right_scanline = round((right[0] - Left_X) / dx);

			left_scanline = max(left_scanline, 0);
			right_scanline = min(right_scanline, width - 1);
			// cerr << left << endl;
			// cerr << right << endl;
			// cerr << left_scanline << ' ' << right_scanline << endl;
			for(int j = left_scanline; j <= right_scanline; j++){
				//calculate Z values
				// cerr << "OK" << endl;
				Tf z = getZ(left, right, Left_X + j * dx);
				// cerr << z << endl;
				if(z < z_buffer[i][j] && z > z_front_limit){
					z_buffer[i][j] = z;
					image.set_pixel(j, i, x.color[0], x.color[1], x.color[2]);
				}
			}
			// cerr << "OK4" << endl;
		}	
	}
	// cerr << "DONE" << endl;
	image.save_image("out.bmp");

	z_buffer_file << fixed << setprecision(6);
	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			if(dcmp(z_buffer[i][j] - z_max) == 0){
				// z_buffer_file << string(8, ' ') << "\t";
			}else{
				z_buffer_file << z_buffer[i][j] << "\t";
			}
		}
		z_buffer_file << "\n";
	}



	return 0;
}
