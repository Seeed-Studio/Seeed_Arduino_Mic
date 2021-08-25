#ifndef FILTERS_H_INCLUDED
#define FILTERS_H_INCLUDED

class  FilterBuHp
{
  public:
    FilterBuHp();
  private:
    float v[2];
  public:
    float step(float x);
};

#endif