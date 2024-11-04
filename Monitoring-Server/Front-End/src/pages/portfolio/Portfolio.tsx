import Grid from '@mui/material/Grid';
import TransactionHistory from 'components/sections/dashboard/transaction-history';

const Portfolio = () => {
  return (
    <Grid container spacing={2.5}>
      <Grid item xs={12} md={12}>
        <TransactionHistory title='Stocks' />
      </Grid>
    </Grid>
  );
};

export default Portfolio;
