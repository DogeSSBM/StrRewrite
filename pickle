#!/usr/bin/env python
# coding: utf-8

# In[1]:


import numpy as np
import matplotlib.pyplot as plt


# In[64]:


def find_children(inp):
    children = []
    for i in range(len(inp) - len(r_from) + 1):
        if inp[i:i+len(r_from)] == r_from:
            children.append(inp[:i] + r_to + inp[i+len(r_from):])
    return children


# In[188]:


r_from, r_to = 'AA', 'B'
inp = 'AABAAA'
world = {}


# In[210]:


world_prev = world.copy()


# In[211]:


if len(world) > 0:
    list_all_children = [c for val in world.values() for c in val['children']]
else:
    list_all_children = [inp]
print(list_all_children)


# In[212]:


for c in list_all_children:
    if c not in world:
        children_of_c = find_children(c)
        world[c] = dict(children=children_of_c, parents=[])


# In[213]:


print(world)


# In[214]:


for parent, val in world.items():
    for child in val['children']:
        if child in world:
            if parent not in world[child]['parents']: world[child]['parents'].append(parent)


# In[215]:


print(world)


# In[216]:


world == world_prev


# In[ ]:
