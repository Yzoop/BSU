﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Xml.Serialization;

namespace BinaryTree
{
    /// <summary>
    /// Класс "Бинарное поисковое дерево"
    /// </summary>
    class Tree
    {
        /// <summary>
        /// Класс "узел БПД"
        /// </summary>
        protected class Item
        {
            /// <summary>
            /// info - значение, хранящееся в узле
            /// lSon, rSon, father - ссылки на левого, правого сына и отца
            /// </summary>
            public int info;
            public Item lSon, rSon, father;
            /// <summary>
            /// Конструктор узла БПД
            /// </summary>
            /// <param name="x">значение, хранящееся в узле</param>
            public Item(int x)
            {
                info = x;
                lSon = rSon = father = null;
            }
        }

        /// <summary>
        /// ссылка на корень дерева
        /// </summary>
        protected Item root;

        /// <summary>
        /// конструктор дерева
        /// </summary>
        public Tree()
        {
            root = null;
        }

        /// <summary>
        /// внутренняя процедура поиска
        /// </summary>
        /// <param name="x">искомое значение</param>
        /// <param name="p">ели найдено - ссылка на соответствующий узел, иначе - ссылка на то место, где остановились</param>
        /// <returns>нашли или нет</returns>
        private bool Find(int x, out Item p)
        {
            p = root;
            Item q = p;
            while (q != null)
            {
                p = q;
                if (q.info == x)
                    return true;
                if (q.info < x)
                    q = q.rSon;
                else
                    q = q.lSon;
            }
            return false;
        }

        /// <summary>
        /// внешняя процедура поиска
        /// </summary>
        /// <param name="x">искомое значение</param>
        /// <returns>нашли или нет</returns>
        public bool Find(int x)
        {
            Item p;
            return Find(x, out p);
        }

        /// <summary>
        /// втавка в БПД
        /// </summary>
        /// <param name="x">вставляемое значение</param>
        /// <returns>смогли вставить или нет</returns>
        public bool Insert(int x)
        {
            Item r, p;
            if (root == null)
            {
                r = new Item(x);
                root = r;
                return true;
            }
            if (Find(x, out r))
                return false;
            p = new Item(x);
            p.father = r;
            if (r.info < x)
                r.rSon = p;
            else
                r.lSon = p;
            return true;
        }

        /// <summary>
        /// удалить вершину (оборвать все ссылки)
        /// </summary>
        /// <param name="x">удаляемая вершина</param>
        private void deleteItem(Item x)
        {
            if (x.father == null)
                if (x.lSon != null)
                {
                    root = x.lSon;
                    x.lSon.father = null;
                }
                else
                {
                    root = x.rSon;
                    if (x.rSon != null)
                        x.rSon.father = null;
                }
            else
            if (x.father.lSon == x)
                if (x.lSon != null)
                {
                    x.father.lSon = x.lSon;
                    x.lSon.father = x.father;
                }
                else
                {
                    x.father.lSon = x.rSon;
                    if (x.rSon != null)
                        x.rSon.father = x.father;
                }
            else
                if (x.lSon != null)
            {
                x.father.rSon = x.lSon;
                x.lSon.father = x.father;
            }
            else
            {
                x.father.rSon = x.rSon;
                if (x.rSon != null)
                    x.rSon.father = x.father;
            }
            x.father = x.lSon = x.rSon = null;
        }

        /// <summary>
        /// Удалить вершину по значению
        /// </summary>
        /// <param name="x">удаляемое значение</param>
        /// <returns>смогли удалить или нет</returns>
        public bool Delete(int x)
        {
            Item r, p;
            if (!Find(x, out r))
                return false;
            if ((r.lSon == null) || (r.rSon == null))
            {
                deleteItem(r);
                return true;
            }
            p = r.lSon;
            while (p.rSon != null)
                p = p.rSon;
            r.info = p.info;
            deleteItem(p);
            return true;
        }

    }

    class MyTree : Tree
    {

        public int min { get; private set; }
        public int max { get; private set; }

        /// <summary>
        /// Нахождение минимального и максимального значенния в BST
        /// </summary>
        /// <returns>нашли или нет</returns>
        private bool FindMinMAx()
        {
            Item q = root;
            if (q != null)
            {
                while (q.rSon != null || q.lSon != null)
                {
                    if (q.rSon != null)
                        q = q.rSon;
                    else
                        q = q.lSon;
                }
                max = q.info;

                q = root;
                while (q.rSon != null || q.lSon != null)
                {
                    if (q.lSon != null)
                        q = q.lSon;
                    else
                        q = q.rSon;
                }
                min = q.info;
                return true;
            }
            return false;

        }


        /// <summary>
        /// Удаление минимального и максимального значенния в BST
        /// </summary>
        /// <returns>удалили или нет</returns>
        public bool DeleteMinMax()
        {
            if (FindMinMAx())
            {
                if (min == max)
                    Delete(min);
                else
                {
                    Delete(min);
                    Delete(max);
                }
                return true;
            }
            return false;
        }




        /// <summary>
        /// Вывод дерева
        /// </summary>
        public void Print()
        {
            if (root != null)
                PrintTree(root, "", true);
            else
                Console.WriteLine("Tree is empty");
        }

        private void PrintTree(Item p, string s, bool last)
        {
            Console.Write(s);
            if (last)
            {
                Console.Write("└─");
                s += "  ";
            }
            else
            {
                Console.Write("├─");
                s += "| ";
            }
            Console.WriteLine(p.info);

            List<Item> ls = new List<Item>();
            if (p.rSon != null)
                ls.Add(p.rSon);
            if (p.lSon != null)
                ls.Add(p.lSon);

            for (int i = 0; i < ls.Count; i++)
                PrintTree(ls[i], s, i == ls.Count - 1);
        }
    }
}
