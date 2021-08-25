#include "filters.h"

//High pass butterworth filter order=1 alpha1=0.0125 
FilterBuHp::FilterBuHp()
{
  v[0]=0.0;
}

float FilterBuHp::step(float x)
{
  v[0] = v[1];
  v[1] = (9.621952458291035404e-1f * x) + (0.92439049165820696974f * v[0]);
  return (v[1] - v[0]);
}
