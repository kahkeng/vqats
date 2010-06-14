template<class T>
T** new_table(int n1, int n2)
{
    T** table = new T*[n1];
    for (uint16_t i1 = 0; i1 < n1; i1++)
        table[i1] = new T[n2];
    return table;
}

template<class T>
T** new_table(int n1, int n2, T initial)
{
    T** table = new T*[n1];
    for (uint16_t i1 = 0; i1 < n1; i1++)
    {
        table[i1] = new T[n2];
        for (uint16_t i2 = 0; i2 < n2; i2++)
            table[i1][i2] = initial;
    }
    return table;
}

template<class T>
void delete_table(T** table, int n1, int n2)
{
    for (uint16_t i = 0; i < n1; i++)
        delete[] table[i];
    delete[] table;
}

