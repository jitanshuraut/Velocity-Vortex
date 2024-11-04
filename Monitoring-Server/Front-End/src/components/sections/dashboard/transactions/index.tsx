import Paper from '@mui/material/Paper';
import Stack from '@mui/material/Stack';
import Button from '@mui/material/Button';
import Typography from '@mui/material/Typography';
import IconifyIcon from 'components/base/IconifyIcon';

interface TransactionProps {
  id: number | string;
  type: string;
  date: string;
  icon: string;
  color: string;
}

const transactions: TransactionProps[] = [
  {
    id: 1,
    type: 'Public Transport',
    date: '22 September 2020',
    icon: 'ic:outline-directions-bus-filled',
    color: 'primary.main',
  },
  {
    id: 2,
    type: 'Grocery Store',
    date: '18 September 2020',
    icon: 'ic:outline-shopping-basket',
    color: 'success.main',
  },
  {
    id: 3,
    type: 'Public Transport',
    date: '22 September 2020',
    icon: 'ic:outline-subscriptions',
    color: 'warning.main',
  },
];

const Transactions = () => {
  return (
    <Paper sx={{ height: 355 }}>
      <Typography mb={3} variant="h5">
        Your transactions
      </Typography>

      {transactions.map((item) => (
        <Stack key={item.id} mb={3} spacing={2} justifyContent="flex-start" alignItems="center">
          <Stack
            alignItems="center"
            justifyContent="center"
            height={48}
            width={48}
            bgcolor="info.dark"
            borderRadius="50%"
          >
            <IconifyIcon icon={item.icon} color={item.color} fontSize="h4.fontSize" />
          </Stack>
          <Stack direction="column">
            <Typography variant="body1" fontWeight={700}>
              {item.type}
            </Typography>
            <Typography variant="caption" color="text.disabled" fontWeight={500}>
              {item.date}
            </Typography>
          </Stack>
        </Stack>
      ))}

      <Stack justifyContent="flex-end">
        <Button
          variant="text"
          size="medium"
          sx={{ mr: -1, color: 'primary.main', fontWeight: 700 }}
          endIcon={<IconifyIcon icon="ic:baseline-east" />}
        >
          View all
        </Button>
      </Stack>
    </Paper>
  );
};

export default Transactions;
