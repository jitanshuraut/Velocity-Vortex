import Grid from '@mui/material/Grid';
import TopCards from 'components/sections/dashboard/top-cards';
import Balance from 'components/sections/dashboard/balance';

const Dashbaord = () => {
  return (
    <Grid container spacing={2.5}>
      <Grid item xs={12}>
        <TopCards />
      </Grid>
      <Grid item xs={12} md={12}>
        <Balance title="Real Time" />
      </Grid>

      <Grid item xs={12} md={12}>
        <Balance title="Trades" />
      </Grid>
    </Grid>
  );
};

export default Dashbaord;
