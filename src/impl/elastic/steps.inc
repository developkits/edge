/**
 * @file This file is part of EDGE.
 *
 * @author Alexander Breuer (anbreuer AT ucsd.edu)
 *
 * @section LICENSE
 * Copyright (c) 2016-2017, Regents of the University of California
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * @section DESCRIPTION
 * Invidual steps of a time group update for the elastic wave equations.
 **/
if( i_step == 0 ) {
#if PP_ORDER == 1
  EDGE_CHECK_NE( i_enSp, nullptr );
  // finite volume: time
  edge::elastic::solvers::FiniteVolume::timePred( i_first,
                                                  i_size,
                                                  m_covSimTime,
                                                  m_dT,
                                                  i_enSp[2].first,
                                                  i_enSp[0].first,
                                                  m_internal.m_elementChars,
                                                  m_internal.m_elementModePrivate1,
                                                  m_internal.m_elementModePrivate2,
                                                  m_internal.m_elementSparseShared3[0],
                                                  io_recvs );
#else
  // ADER-DG local
  edge::elastic::solvers::AderDg::local( i_first,
                                         i_size,
                                         m_covSimTime,
                                         m_dT,
                                         i_enSp[2].first,
                                         i_enSp[0].first,
                                         m_internal.m_connect.elFa,
                                         m_internal.m_faceChars,
                                         m_internal.m_elementChars,
                                         m_internal.m_globalShared1[0],
                                         m_internal.m_elementShared4,
                                         m_internal.m_elementShared2,
                                         m_internal.m_elementModePrivate1,
                                         m_internal.m_elementModePrivate2,
                                         m_internal.m_elementSparseShared3[0],
                                         io_recvs
#ifdef PP_T_KERNELS_XSMM
                                       ,&m_internal.m_xsmm.m_kernels[0]
#elif defined PP_T_KERNELS_XSMM_DENSE_SINGLE
                                       ,&m_internal.m_xsmm.m_kernels[0]
#endif
                                       );
#endif
}
else if( i_step == 1 ) {
#if PP_ORDER == 1
  // finite volume: update
  edge::elastic::solvers::FiniteVolume::update( i_first,
                                                i_size,
                                                i_enSp[0].first,
                                                m_internal.m_connect.elFaEl,
                                                m_internal.m_connect.elFa,
                                                m_internal.m_faceSparseShared3,
                                                m_internal.m_elementSparseShared2[0],
                                                m_internal.m_faceChars,
                                                m_internal.m_elementShared2,
                                                m_internal.m_elementShared3,
                                                m_internal.m_elementModePrivate2,
                                                m_internal.m_faceSparseShared4,
                                                m_internal.m_elementModePrivate1 );
#else
  // ADER-DG: neigh contrib
  edge::elastic::solvers::AderDg::neigh( i_first,
                                         i_size,
                                         i_enSp[0].first,
                                         m_internal.m_globalShared1[0],
                                         m_internal.m_faceChars,
                                         m_internal.m_elementShared3,
                                         m_internal.m_connect.elFa,
                                         m_internal.m_connect.elFaEl,
                                         m_internal.m_faceSparseShared3,
                                         m_internal.m_elementSparseShared2[0],
                                         m_internal.m_connect.fIdElFaEl,
                                         m_internal.m_connect.vIdElFaEl,
                                         m_internal.m_elementModePrivate2,
                                         m_internal.m_faceSparseShared4,
                                         m_internal.m_elementModePrivate1
#ifdef PP_T_KERNELS_XSMM
                                       ,&m_internal.m_xsmm.m_kernels[ORDER*(N_DIM+1)]
#elif defined PP_T_KERNELS_XSMM_DENSE_SINGLE
                                       ,&m_internal.m_xsmm.m_kernels[0]
#endif
                                       );
#endif
}
else if ( i_step == 2 ) {
#ifndef PP_T_EQUATIONS_ELASTIC_RUPTURE
  // apply sources
  edge::elastic::solvers::Kinematics< T_SDISC.ELEMENT,
                                      N_QUANTITIES,
                                      ORDER,
                                      N_CRUNS,
                                      1 >::applyDirac(              i_first,
                                                                    i_size,
                                                       (real_base) (m_covSimTime),
                                                       (real_base) (m_covSimTime+m_dT),
                                                                    m_internal.m_globalShared4[0],
                                                                    m_internal.m_globalShared3,
                                                                    m_internal.m_elementModePrivate1 );

#else
#if PP_ORDER == 1
  edge::elastic::solvers::FiniteVolume::rupture( i_first,
                                                 i_size,
                                                 i_enSp[0].first,
                                                 m_covSimTime,
                                                 m_dT,
                                                 m_internal.m_faceSparseShared3,
                                                 m_internal.m_faceSparseShared6[0],
                                                 m_internal.m_globalShared2[0],
                                                 m_internal.m_faceSparseShared1[0],
                                                 m_internal.m_faceSparseShared2[0],
                                                 m_internal.m_faceSparseShared5,
                                                 m_internal.m_elementSparseShared3[0],
                                                 m_internal.m_faceSparseShared4,
                                                 io_recvsQuad );
#else
  edge::elastic::solvers::AderDg::rupture( i_first,
                                           i_size,
                                           i_enSp[0].first,
                                           m_covSimTime,
                                           m_dT,
                                           m_internal.m_globalShared1[0],
                                           m_internal.m_elementShared4,
                                           m_internal.m_faceSparseShared3,
                                           m_internal.m_faceSparseShared6[0],
                                           m_internal.m_globalShared2[0],
                                           m_internal.m_faceSparseShared1[0],
                                           m_internal.m_faceSparseShared2[0],
                                           m_internal.m_faceSparseShared5,
                                           m_internal.m_elementSparseShared3[0],
                                           m_internal.m_faceSparseShared4,
                                           io_recvsQuad
#ifdef PP_T_KERNELS_XSMM
                                       ,&m_internal.m_xsmm.m_kernels[ORDER*(N_DIM+1)]
#elif defined PP_T_KERNELS_XSMM_DENSE_SINGLE
                                       ,&m_internal.m_xsmm.m_kernels[0]
#endif
                                       );
#endif

#endif
}
else EDGE_LOG_FATAL << "step not supported in elastic implementation: " << i_step;
