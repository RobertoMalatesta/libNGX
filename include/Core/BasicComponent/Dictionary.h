//===- Dictionary.h - Dictionary to provide quick data index --*- C++ -*-===//
//
//                     The NGX Server Infrastructure
//
// This file is distributed under the MIT Open Source License. See LICENSE.TXT
// for detail.
//
//===----------------------------------------------------------------------===//
//
//  This file provide Dictionary to manage and index data quickly
//
//===----------------------------------------------------------------------===//

/**
*  @brief DictionaryItem
*
*  DictionaryItem should have key and hashable
*
*/
class DictionaryItem : public RBNode, public Achor {

protected:

    const char *Key;
    uint32_t Hash = 0;

    // DictionaryItem compare
    virtual int32_t operator-(DictionaryItem &R);

    // Hash compare
    virtual int32_t operator-(uint32_t RHash);

    /** HashFn
     *  define how to calc Hash
     */
    virtual uint32_t HashFn() const;

public:

    DictionaryItem(const char *Key);

    /** GetHash
     *
     *  get hash of current item
     *
     *  @return Hash value
     */
    inline uint32_t GetHash() {

        if (Hash == 0) {
            Hash = HashFn();
        }

        return Hash;
    }

    // implement RBNode operator - to compare
    virtual int operator-(RBNode &R);
};

/**
 *  @brief Dictionary
 *
 *  Dictionary is designed to store and index key-value data within log(N) time
 *
 */
class Dictionary : public RBTree {

public:

    Dictionary() = default;

    ~Dictionary();

    /** Insert
     *
     *  add a new DictionaryItem to this Dictionary,
     *
     *  @param I Item to insert;
     *  @return 1 if succeed, 0 if duplicated item find
     */
    int Insert(DictionaryItem &I);

    /** Remove
     *
     *  remove a DictionaryItem from this Dictionary,
     *
     *  @param I Item to remove;
     *  @return 1 if succeed, 0 if not found
     */
    int Remove(DictionaryItem &I);

    /** Find
     *
     *  find a item with input hash in this dictionary, so hash collision may occur.
     *
     *  @param Hash target hash;
     *  @return nullptr if not found, or pointer to the dictionary item
     */
    DictionaryItem *Find(uint32_t Hash);

    /** Find
     *
     *  find a item with totally same key as input.
     *
     *  @param Item target input with key specified, will calculate hash when compare;
     *  @return nullptr if not found, or pointer to the dictionary item
     */
    DictionaryItem *Find(DictionaryItem Item);
};
