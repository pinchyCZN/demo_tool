void physics(SURFACE *s,PLAYER *b)
{
	int cx,cy;
	float vx,vy,vz;
	static float lastz=0,lastvz=0,time=0;
	cx=s->width/2;
	cy=s->length/2;
	//if(b->v[0]>0.000001 || b->v[2]!=0)
	{
		vx=b->v[0];
		vy=b->v[1];
		vz=b->v[2];
		vx-=.0002;//linear reduce
		if(vx<0)
			vx=0;
		s->x+=vx; //move the surface
		if(s->data[cx][cy][2]>b->z){ //below surface
			float dz=lastz-s->data[cx][cy][2]; //delta z
			//dz=sqrt(dz);
			vz=sqrt(dz*dz+vx*vx+vy*vy)/10;
			b->z=s->data[cx][cy][2];
			time=0;
		}
		else{ //free fall
			vz-=2*time;
			time+=1;
			b->z+=vz;
			if(b->z<s->data[cx][cy][2])
				b->z=s->data[cx][cy][2];
		}
		lastz=b->z;
		lastvz=vz;
		b->v[0]=vx;
		b->v[1]=vy;
		b->v[2]=vz;
	}
	printf("vz=%f vx=%f bz=%f\n",vz,vx,b->z-s->data[cx][cy][2]);

}