interface RowsProps {
  id: number | string;
  category: string;
  date: string;
  amount: string;
  paymentMethod: string;
  status: string;
  balance: string;
}

export const rows: RowsProps[] = [
  {
    id: 1001,
    category: 'Groceries',
    date: '2024-08-20',
    amount: '150$',
    paymentMethod: 'Credit Card',
    status: 'Completed',
    balance: '1201$',
  },
  {
    id: 1002,
    category: 'Utilities',
    date: '2024-08-21',
    amount: '175$',
    paymentMethod: 'Bank Transfer',
    status: 'Pending',
    balance: '1126$',
  },
  {
    id: 1003,
    category: 'Dining',
    date: '2024-08-22',
    amount: '120$',
    paymentMethod: 'Debit Card',
    status: 'Completed',
    balance: '1081$',
  },
  {
    id: 1004,
    category: 'Transport',
    date: '2024-08-23',
    amount: '185$',
    paymentMethod: 'Cash',
    status: 'Completed',
    balance: '1066$',
  },
  {
    id: 1005,
    category: 'Entertainment',
    date: '2024-08-24',
    amount: '195$',
    paymentMethod: 'Credit Card',
    status: 'Failed',
    balance: '1066$',
  },
  {
    id: 1006,
    category: 'Shopping',
    date: '2024-08-25',
    amount: '150$',
    paymentMethod: 'Credit Card',
    status: 'Completed',
    balance: '916$',
  },
  {
    id: 1007,
    category: 'Rent',
    date: '2024-08-26',
    amount: '800$',
    paymentMethod: 'Bank Transfer',
    status: 'Completed',
    balance: '116$',
  },
  {
    id: 1008,
    category: 'Salary',
    date: '2024-08-27',
    amount: '2000$',
    paymentMethod: 'Direct Deposit',
    status: 'Completed',
    balance: '2116$',
  },
  {
    id: 1009,
    category: 'Investment',
    date: '2024-08-28',
    amount: '500$',
    paymentMethod: 'Bank Transfer',
    status: 'Pending',
    balance: '1616$',
  },
  {
    id: 1010,
    category: 'Healthcare',
    date: '2024-08-29',
    amount: '120$',
    paymentMethod: 'Debit Card',
    status: 'Completed',
    balance: '1496$',
  },
  {
    id: 1011,
    category: 'Insurance',
    date: '2024-08-30',
    amount: '250$',
    paymentMethod: 'Bank Transfer',
    status: 'Completed',
    balance: '1246$',
  },
  {
    id: 1012,
    category: 'Savings',
    date: '2024-08-31',
    amount: '300$',
    paymentMethod: 'Bank Transfer',
    status: 'Completed',
    balance: '946$',
  },
];
