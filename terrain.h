

//http://freespace.virgin.net/hugo.elias/models/m_perlin.htm
float lerp(float a,float b,float x)
{
	return a*(1-x)+b*x;
}
float cos_interp(float a,float b,float x)
{
	float ft,f;
	ft=x*pi;
	f=(1-cos(ft))*0.5;
	return a*(1-f)+b*f;
}
float cubic_interp(float v0, float v1,float v2,float v3,float x)
{
	float P,Q,R,S;
	P = (v3 - v2) - (v0 - v1);
	Q = (v0 - v1) - P;
	R = v2 - v0;
	S = v1;
	return P*pow(x,3) + Q*pow(x,2) + R*x + S;
}
float noise(int x,int y)
{
	int n=x+y*57;
	n=(n<<13)^n;
	return (1.0-((n*(n*n*15731+789221)+1376312589)&0x7FFFFFFF)/1073741824.0);
}

float smooth_noise(int x,int y)
{
	float corners,sides,center;
	corners=(noise(x-1,y-1)+noise(x+1,y-1)+noise(x-1,y+1)+noise(x+1,y+1))/16;
	sides=(noise(x-1,y)+noise(x+1,y)+noise(x,y-1)+noise(x,y+1))/8;
	center=noise(x,y)/4;
	return corners+sides+center;
}
float interp_noise(float x,float y)
{
	double intX,intY,fracX,fracY;
	double v1,v2,v3,v4,i1,i2;
	fracX=modf(x,&intX);
	fracY=modf(y,&intY);
	v1=smooth_noise((int)intX,(int)intY);
	v2=smooth_noise((int)intX+1,(int)intY);
	v3=smooth_noise((int)intX,(int)intY+1);
	v4=smooth_noise((int)intX+1,(int)intY+1);
	//return cubic_interp(v1,v2,v3,v4,fracX+fracY);
	i1=cos_interp(v1,v2,fracX);
	i2=cos_interp(v3,v4,fracX);
	return cos_interp(i1,i2,fracY);
}
float perlin_noise2d(float x,float y,float persistance,int octaves)
{
	float total;
	float freq,ampl,p;
	int i,n;
	total=0;
	p=persistance;
	n=octaves;
	for(i=0;i<n;i++){
		freq=pow(2,i+1);
		ampl=pow(p,i+1);
		total=total+interp_noise(x*freq,y*freq)*ampl;
	}
	return total;
}
float terrain_map[100][100][3];

int draw_bike(PLAYER *b)
{
	glPushMatrix();
	glRotatef(b->rx,1,0,0);
	glRotatef(b->ry,0,1,0);
	glRotatef(b->rz,0,0,1);
	glTranslatef(b->x,b->y,b->z);
	glRotatef(90,1,0,0);
	glutSolidTorus(2,4,10,10);
	glTranslatef(-15,0,0);
	glutSolidTorus(2,4,10,10);
//	glutWireTorus(10,20,20,20);
	glPopMatrix();
	return TRUE;	
}
int draw_terrain(SURFACE *s)
{
	int i,j;
	glBegin(GL_POINTS);
	for(i=0;i<s->width;i++){
		for(j=0;j<s->length;j++){
			glVertex3f(s->data[i][j][0],s->data[i][j][1],s->data[i][j][2]);
		}
	}
	glEnd();
	return TRUE;	
}
int create_terrain(SURFACE *s,int width,int height,float persistance,float octaves)
{
	int i,j;
	float scale=10;
	float cx,cy;
	cy=width/2;
	cx=height/2;

	for(i=0;i<width;i++){
		for(j=0;j<height;j++){
			float x,y,h;
			x=i;y=j;
			x=x/100;
			y=y/100;
			h=perlin_noise2d(x+s->x,y+s->y,persistance,octaves);
			if(i<MAX_TERRAIN_DIM && j<MAX_TERRAIN_DIM){
				s->data[i][j][0]=(i-cx)*scale;
				s->data[i][j][1]=(j-cy)*scale;
				s->data[i][j][2]=h*scale;
			}
		}
	}
	return TRUE;
}

