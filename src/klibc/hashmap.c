#include "hashmap.h"
#include "klibc/stdlib.h"
#include "klibc/linked_list.h"
#include "klibc/string.h"

static uint64_t get_bucket_from_hash(uint64_t hash) {
    return hash % HASHMAP_BUCKET_SIZE;
}

hashmap_t *init_hashmap() {
    hashmap_t *ret = kcalloc(sizeof(hashmap_t));
    return ret;
}

void delete_hashmap(hashmap_t *hashmap) {
    for (uint64_t i = 0; i < HASHMAP_BUCKET_SIZE; i++) {
        hashmap_bucket_t bucket = hashmap->buckets[i];
        hashmap_elem_t *cur_element = bucket.elements;

        if (hashmap->buckets[i].elements) {
            while (cur_element->next) {
                cur_element = cur_element->next;
            }

            while (cur_element->prev) {
                cur_element = cur_element->prev;
                kfree(cur_element->next);
            }
        }
    }
    kfree(hashmap->buckets);
    kfree(hashmap);
}

static hashmap_elem_t *hashmap_get_elem_dat(hashmap_t *hashmap, uint64_t key) {
    lock(hashmap->hashmap_lock);
    hashmap_elem_t *ret = (hashmap_elem_t *) 0;

    uint64_t bucket = get_bucket_from_hash(key);
    hashmap_elem_t *cur_elem = hashmap->buckets[bucket].elements;
    while (cur_elem) {
        if (cur_elem->key == key && atomic_inc(&cur_elem->ref_count)) {
            ret = cur_elem;
            goto done;
        }

        cur_elem = cur_elem->next;
    }

done:
    unlock(hashmap->hashmap_lock);
    return ret;
}

static void hashmap_unref_elem(hashmap_elem_t *elem) {
    if (!atomic_dec(&elem->ref_count)) {
        // If no more refs exist, remove the element
        UNCHAIN_LINKED_LIST(elem);
    }
}

void hashmap_remove_elem(hashmap_t *hashmap, uint64_t key) {
    hashmap_elem_t *elem = hashmap_get_elem_dat(hashmap, key);
    if (elem) {
        hashmap_unref_elem(elem); // Unref for the time we referenced the element to get it
        hashmap_unref_elem(elem); // Unref for the 1 we set refcount to when the element is created
    }
} 

void *hashmap_get_elem(hashmap_t *hashmap, uint64_t key) {
    hashmap_elem_t *elem = hashmap_get_elem_dat(hashmap, key);
    if (elem) {
        void *data = elem->data;
        hashmap_unref_elem(elem);
        return data;
    } else {
        return (void *) 0;
    }
}

void hashmap_set_elem(hashmap_t *hashmap, uint64_t key, void *data) {
    hashmap_elem_t *elem = hashmap_get_elem_dat(hashmap, key);
    lock(hashmap->hashmap_lock);
    if (elem) {
        elem->data = data;
        hashmap_unref_elem(elem);
    } else {
        uint64_t bucket = get_bucket_from_hash(key);
        hashmap_elem_t *elem = kcalloc(sizeof(hashmap_elem_t));
        elem->data = data;
        elem->key = key;
        elem->ref_count = 1;

        if (hashmap->buckets[bucket].elements) {
            CHAIN_LINKED_LIST(hashmap->buckets[bucket].elements, elem);
        } else {
            hashmap->buckets[bucket].elements = elem;
        }
    }
    unlock(hashmap->hashmap_lock);
}