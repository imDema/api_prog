# Structures

## Hashtables (chained, ideally dynamic)
### Relation Hashtable `link_ht`
+ `hash = h(id_first,id_second)`, alphabetical order
+ Entry:
    + Arraylist: `arraylist[k]` contains data for the `k`th relation (in order of addition)
        + `arraylist[k] = (0b01 if k(a->b)) | (0b10 if k(a<-b))`

### Entity Hastable `ent_ht`
+ `hash = h(id_ent)`
+ Entry:
    + Linked list (?Arraylist?) : `linklist` of links which contains pointers to `link_ht` entries which have at least an outbound relation from `id_ent`
    + Arraylist: `arraylist[k]` contains the number of entering relations for the `k`th relation (in order of addition)
+ Used for entity deletion and max tracking

### Relation list
+ `count` parameter for number of relations
+ Entry:
    + `index` nonce id for relations
    + `id_rel`
    + Link to top list

## Auxilliary
### Top Cache

+ One for each relation
+ Store top `k` elements in ordered array
+ On relation addition update this checking if new entering relations number for the entity is greater than the min element in top list, if there is space add it, else skip
+ If at any point after first insertion the cache is empty invalidate it and mark it for rebuilding using linear max search during next `report`